#include "NeuralNetwork.hpp"
#include <map>

NeuralNetwork::NeuralNetwork(const Genome& genome){
    std::vector<NodeGene> sortedNodes = genome.getNodes();
    std::sort(sortedNodes.begin(), sortedNodes.end(), [](const NodeGene& a, const NodeGene& b) {return a.layerX < b.layerX;});

    std::map<int, int> idToIndex;
    for(int i{}; i < sortedNodes.size(); i++){
        Neuron newNeuron(sortedNodes[i].nodeType, 0.0f);
        neurons_.push_back(newNeuron);
        idToIndex.insert({sortedNodes[i].nodeID, i});
    }

    for (const auto& connection : genome.getConnections()){
        if (connection.isEnabled){
            int inID = connection.inNodeID;
            int outID = connection.outNodeID;
            if (idToIndex.find(inID) != idToIndex.end() && idToIndex.find(outID) != idToIndex.end()){
                int newInID = idToIndex[inID];
                int newOutID = idToIndex[outID];

                Synapse newSynapse(newInID, newOutID, connection.weight);
                synapses_.push_back(newSynapse);
            }
        }
    }
}

std::vector<float> NeuralNetwork::feedForward(const std::vector<float>& inputs){
    for (auto& n : neurons_){
        n.value = 0.0f;
    }

    int inputIndex = 0;
    for (int i{}; i < neurons_.size(); i++){
        if (neurons_[i].type == NodeType::INPUT && inputIndex < inputs.size()){
            neurons_[i].value = inputs[inputIndex];
            inputIndex++;
        } else if (neurons_[i].type == NodeType::BIAS){
            neurons_[i].value = 1.0f;
        }
    }

    for (int i{}; i < neurons_.size(); i++){
        if (neurons_[i].type == NodeType::HIDDEN || neurons_[i].type == NodeType::OUTPUT){
            neurons_[i].value = sigmoid(neurons_[i].value);
        }
        for (const auto& synapse : synapses_){
            if (synapse.inNeuronID == i){
                neurons_[synapse.outNeuronID].value += neurons_[i].value * synapse.weight;
            }
        }
    }

    std::vector<float> outputs;
    outputs.reserve(16);
    for (const auto& neuron : neurons_){
        if (neuron.type == NodeType::OUTPUT){
            outputs.push_back(neuron.value);
        }
    }

    return outputs;
}