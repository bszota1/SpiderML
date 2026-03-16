#pragma once

#include "Neuron.hpp"
#include "Synapse.hpp"
#include "../genes/Genome.hpp"
#include <vector>
#include <cmath>


class NeuralNetwork{
private:
    std::vector<Neuron> neurons_;
    std::vector<Synapse> synapses_;
public:
    NeuralNetwork(const Genome& genome);
    std::vector<float> feedForward(const std::vector<float>& inputs);
    

    static float sigmoid(float x) {
        return 1.0f / (1.0f + std::exp(-x));
    }
};