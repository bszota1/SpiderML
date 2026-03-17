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
public:
    Population(int size);
    void speciate();
    void calculateFitness();
    void nextGeneration();

    std::vector<Genome>& getPopulationMutable() { return population_; }
    const std::vector<Genome>& getPopulation() const { return population_; }
    int getGeneration() const { return generation_; }
    int getSpeciesCount() const;

};