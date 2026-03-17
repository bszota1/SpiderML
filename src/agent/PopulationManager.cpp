#include "PopulationManager.hpp"

PopulationManager::PopulationManager(int populSize, b2WorldId worldID) : 
    worldID_{worldID},
    populationSize_{populSize},
    generation_{1},
    timer_{0.0f}{
        rng_ = std::mt19937(std::random_device{}());
        createInitialPopulation();
    }

void PopulationManager::createInitialPopulation() {
    Vector2 startPosition = { 200.0f, 400.0f };
    for (int i = 0; i < populationSize_; i++) {
        Genome newGenome(i + 1);
        
        for (int j = 0; j < 5; j++) newGenome.mutateWeights(rng_);
        for (int j = 0; j < 3; j++) newGenome.mutateAddConnection(rng_, innoManager_);

        spiders_.emplace_back(newGenome, startPosition, worldID_);
    }
}

void PopulationManager::nextGeneration() {
    Genome bestGenome = spiders_[0].getGenome(); 
    float maxFitness = spiders_[0].getFitness();

    for (const auto& spider : spiders_) {
        if (spider.getFitness() > maxFitness) {
            maxFitness = spider.getFitness();
            bestGenome = spider.getGenome();
        }
    }
    spiders_.clear();

    Vector2 startPosition = { 200.0f, 400.0f };

    for (int i = 0; i < populationSize_; i++) {
        Genome childGenome = bestGenome;
        childGenome.genomeID_ = i + 1;

        if (i > 0) {
            childGenome.mutateWeights(rng_);
            
            if (i % 3 == 0) {
                childGenome.mutateAddConnection(rng_, innoManager_);
            }
        }

        spiders_.emplace_back(childGenome, startPosition, worldID_);
    }
}

void PopulationManager::update(float deltaTime){
    timer_ += deltaTime;
    for (auto& spider : spiders_){
        spider.update();
    }

    if (timer_ >= generationTime_){
        generation_++;
        timer_ = 0.0f;
        nextGeneration();
    }
}

void PopulationManager::draw() const {
    for (const auto& spider : spiders_){
        spider.draw();
    }
}