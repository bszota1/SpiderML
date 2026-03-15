#include "Population.hpp"

Population::Population(int size) : populationSize_{size} , generation_{0} {
    std::uniform_real_distribution<float> dist(-1.0f,1.0f);
    
    std::vector<NodeGene> nodes;
    nodes.reserve(TOTAL_NODES);
    for (int i{}; i < INPUT_NODES; i++){
        nodes.emplace_back(NodeGene(i, NodeType::INPUT, 0));
    }
    nodes.emplace_back(NodeGene(INPUT_NODES, NodeType::BIAS, 0));
    for (int i{INPUT_NODES + 1}; i < TOTAL_NODES; i++){
        nodes.emplace_back(NodeGene(i, NodeType::OUTPUT, 1));
    }

    int totalConnections = (INPUT_NODES + 1) * OUTPUT_NODES;
    std::vector<ConnectionGene> connecitons;
    connecitons.reserve(totalConnections);
    for(int i{}; i < INPUT_NODES + 1; i++){
        for(int j{}; j < OUTPUT_NODES; j++){
            int inID = i;
            int outID = j + INPUT_NODES + 1;
            connecitons.emplace_back(ConnectionGene(inID, outID, dist(rng_), true, innovManager_.getInnovationID(inID,outID)));
        }
    }

    population_.reserve(populationSize_);
    for (int i{}; i < populationSize_; i++){
        population_.emplace_back(Genome(i, nodes, connecitons));
    }

}

void Population::speciate(){
    for (auto& species : species_){
        species.members.clear();
    }

    for (int i{}; i < population_.size(); i++){
        Genome& currentSpider = population_[i];
        bool foundSpecies = false;
        for (auto& species : species_){
            float currentDist = currentSpider.getCompabilityDistance(species.representative, 1.0f, 1.0f, 0.4f);
            if (currentDist < COMPATIBILITY_THRESHOLD){
                currentSpider.speciesID_ = species.speciesID;
                species.members.push_back(&currentSpider);
                foundSpecies = true;
                break;
            }
        }
        if (!foundSpecies){
                Species newSpecies(species_.size(), currentSpider);

                newSpecies.members.push_back(&currentSpider);
                currentSpider.speciesID_ = newSpecies.speciesID;
                species_.push_back(newSpecies);
            }
    }
}

void Population::calculateFitness(){
    for (auto& species : species_){
        int speciesSize = species.members.size();
        if (speciesSize == 0) continue;

        for (Genome* memberPtr : species.members){
            memberPtr->adjustedFitness_ = memberPtr->fitness_ / speciesSize;
        }
    }
}

void Population::nextGeneration() {
    std::vector<Genome> nextGen;
    nextGen.reserve(populationSize_);

    float globalAdjustedFitness = 0.0f;
    Genome* absoluteBest = nullptr;

    for (auto& species : species_) {
        if (species.members.empty()) continue;

        std::sort(species.members.begin(), species.members.end(), [](const Genome* a, const Genome* b) {
            return a->fitness_ > b->fitness_;
        });

        if (absoluteBest == nullptr || species.members[0]->fitness_ > absoluteBest->fitness_) {
            absoluteBest = species.members[0];
        }

        int survivalCount = std::max(1, (int)species.members.size() / 2);
        species.members.resize(survivalCount);

        for (auto* member : species.members) {
            globalAdjustedFitness += member->adjustedFitness_;
        }
    }

    
    if (absoluteBest != nullptr) {
        Genome champion = *absoluteBest;
        champion.genomeID_ = nextGen.size();
        nextGen.push_back(champion);
    }

    std::uniform_real_distribution<float> distProb(0.0f, 1.0f);

    for (auto& species : species_) {
        if (species.members.empty()) continue;

        float speciesFitness = 0.0f;
        for (auto* member : species.members) {
            speciesFitness += member->adjustedFitness_;
        }

        int offspringCount = 0;
        if (globalAdjustedFitness > 0.0f) {
            offspringCount = std::floor((speciesFitness / globalAdjustedFitness) * populationSize_);
        }

        std::uniform_int_distribution<int> parentDist(0, species.members.size() - 1);

        for (int i = 0; i < offspringCount; i++) {
            if (nextGen.size() >= populationSize_) break;

            Genome* parent1 = species.members[parentDist(rng_)];
            Genome* parent2 = species.members[parentDist(rng_)];

            if (parent2->fitness_ > parent1->fitness_) {
                std::swap(parent1, parent2);
            }

            Genome child = Genome::crossover(nextGen.size(), *parent1, *parent2, rng_);

            if (distProb(rng_) < 0.80f) child.mutateWeights(rng_);
            if (distProb(rng_) < 0.05f) child.mutateAddConnection(rng_, innovManager_);            
            if (distProb(rng_) < 0.01f) child.mutateAddNode(rng_, innovManager_);

            nextGen.push_back(child);
        }
    }

    while (nextGen.size() < populationSize_) {
        Genome backupChild = *absoluteBest;
        backupChild.genomeID_ = nextGen.size();
        backupChild.mutateWeights(rng_);
        nextGen.push_back(backupChild);
    }

    population_ = std::move(nextGen);
    generation_++;
}