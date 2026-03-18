#include "Population.hpp"
#include "../evolution/NeatConfig.hpp"

Population::Population(int size) : populationSize_{size} , generation_{0} {
    std::uniform_real_distribution<float> dist(-1.0f,1.0f);
    std::uniform_real_distribution<float> distProb(0.0f, 1.0f);
    
    std::vector<NodeGene> nodes;
    nodes.reserve(TOTAL_NODES);
    for (int i{}; i < INPUT_NODES; i++){
        nodes.emplace_back(NodeGene(i, NodeType::INPUT, 0));
    }
    nodes.emplace_back(NodeGene(INPUT_NODES, NodeType::BIAS, 0));
    for (int i{INPUT_NODES + 1}; i < TOTAL_NODES; i++){
        nodes.emplace_back(NodeGene(i, NodeType::OUTPUT, 1));
    }

    population_.reserve(populationSize_);
    for (int i{}; i < populationSize_; i++){
        std::vector<ConnectionGene> genomeConnections;

        for (int out = 0; out < OUTPUT_NODES; out++) {
            int forcedIn = std::uniform_int_distribution<int>(0, INPUT_NODES)(rng_);
            int inID = forcedIn;
            int outID = out + INPUT_NODES + 1;
            genomeConnections.emplace_back(ConnectionGene(inID, outID, dist(rng_), true, innovManager_.getInnovationID(inID, outID)));
        }

        for(int in = 0; in < INPUT_NODES + 1; in++){
            for(int out = 0; out < OUTPUT_NODES; out++){
                int inID = in;
                int outID = out + INPUT_NODES + 1;

                bool exists = false;
                for (const auto& connection : genomeConnections) {
                    if (connection.inNodeID == inID && connection.outNodeID == outID) {
                        exists = true;
                        break;
                    }
                }
                if (exists) {
                    continue;
                }

                if (distProb(rng_) < NeatConfig::kInitialConnectionProbability) {
                    genomeConnections.emplace_back(ConnectionGene(inID, outID, dist(rng_), true, innovManager_.getInnovationID(inID, outID)));
                }
            }
        }

        population_.emplace_back(Genome(i, nodes, genomeConnections));
    }

}

void Population::speciate(){
    species_.erase(std::remove_if(species_.begin(), species_.end(), [](const Species& species) {
        return species.members.empty();
    }), species_.end());

    for (size_t i = 0; i < species_.size(); ++i) {
        species_[i].speciesID = static_cast<int>(i);
    }

    for (auto& species : species_){
        species.members.clear();
    }

    for (int i{}; i < population_.size(); i++){
        Genome& currentSpider = population_[i];
        bool foundSpecies = false;
        for (auto& species : species_){
            float currentDist = currentSpider.getCompabilityDistance(
                species.representative,
                NeatConfig::kCompatibilityC1,
                NeatConfig::kCompatibilityC2,
                NeatConfig::kCompatibilityC3
            );
            if (currentDist < NeatConfig::kCompatibilityThreshold){
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

    for (auto& species : species_) {
        if (!species.members.empty()) {
            species.representative = *species.members.front();
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
    std::vector<Genome*> allCandidates;
    allCandidates.reserve(population_.size());

    for (auto& species : species_) {
        if (species.members.empty()) continue;

        std::sort(species.members.begin(), species.members.end(), [](const Genome* a, const Genome* b) {
            return a->fitness_ > b->fitness_;
        });

        allCandidates.insert(allCandidates.end(), species.members.begin(), species.members.end());

        int survivalCount = std::max(1, (int)species.members.size() / 2);
        species.members.resize(survivalCount);

        for (auto* member : species.members) {
            globalAdjustedFitness += member->adjustedFitness_;
        }
    }

    std::sort(allCandidates.begin(), allCandidates.end(), [](const Genome* a, const Genome* b) {
        return a->fitness_ > b->fitness_;
    });

    const int eliteCount = std::min({NeatConfig::kElitismCount, populationSize_, static_cast<int>(allCandidates.size())});
    for (int i = 0; i < eliteCount; i++) {
        Genome elite = *allCandidates[i];
        elite.genomeID_ = nextGen.size();
        nextGen.push_back(elite);
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

            if (distProb(rng_) < NeatConfig::kGenomeMutateWeightsChance) child.mutateWeights(rng_);
            if (distProb(rng_) < NeatConfig::kAddConnectionChance) child.mutateAddConnection(rng_, innovManager_);
            if (distProb(rng_) < NeatConfig::kAddNodeChance) child.mutateAddNode(rng_, innovManager_);

            nextGen.push_back(child);
        }
    }

    const int backupPoolSize = std::min<int>(std::max<int>(1, static_cast<int>(allCandidates.size()) / 10), static_cast<int>(allCandidates.size()));
    std::uniform_int_distribution<int> backupDist(0, backupPoolSize - 1);

    while (nextGen.size() < populationSize_ && !allCandidates.empty()) {
        Genome backupChild = *allCandidates[backupDist(rng_)];
        backupChild.genomeID_ = nextGen.size();
        backupChild.mutateWeights(rng_);
        nextGen.push_back(backupChild);
    }

    population_ = std::move(nextGen);
    generation_++;
}

int Population::getSpeciesCount() const {
    int count = 0;
    for (const auto& species : species_) {
        if (!species.members.empty()) {
            count++;
        }
    }
    return count;
}