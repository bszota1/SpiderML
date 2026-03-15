#pragma once

#include "NodeGene.hpp"
#include "ConnectionGene.hpp"
#include "InnovationManager.hpp"
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

class Population;

class Genome{
private:
    std::vector<NodeGene> nodes_;
    std::vector<ConnectionGene> connections_;
public:
    float fitness_;
    float adjustedFitness_;
    int genomeID_;
    int speciesID_;

    Genome(int id);
    Genome(int id, const std::vector<NodeGene> genes, const std::vector<ConnectionGene> connections);

    const std::vector<NodeGene>& getNodes() const { return nodes_; }
    const std::vector<ConnectionGene>& getConnections() const { return connections_; }

    void mutateWeights(std::mt19937& rng);
    void mutateAddConnection(std::mt19937& rng, InnovationManager& innovManager);
    void mutateAddNode(std::mt19937& rng, InnovationManager& innovManager);

    static Genome crossover(int childID, const Genome& parent1, const Genome& parent2, std::mt19937& rng);
    float getCompabilityDistance(const Genome& other, float c1, float c2, float c3) const;

}; 