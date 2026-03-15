#pragma once
#include "Genome.hpp"
#include "InnovationManager.hpp"

struct Species{
    int speciesID;
    Genome representative;
    std::vector<Genome*> members;
    float topFitness;
    int staleness;

    Species(int id, const Genome& rep) : 
        speciesID{id}, 
        representative{rep}, 
        topFitness{0.0f}, 
        staleness{0} 
    {}
};

class Population{
private:
    int populationSize_;
    int generation_;
    std::vector<Genome> population_;
    std::vector<Species> species_;
    InnovationManager innovManager_;
    std::mt19937 rng_;
    static constexpr float COMPATIBILITY_THRESHOLD = 3.0f;
public:
    Population(int size);
    void speciate();
    void calculateFitness();
    void nextGeneration();

    const std::vector<Genome>& getPopulation() const { return population_; }

};