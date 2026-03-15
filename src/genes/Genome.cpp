#include "Genome.hpp"

Genome::Genome(int id) :
    genomeID_{id},
    fitness_{0.0f},
    adjustedFitness_{0.0f},
    speciesID_{-1}{}

Genome::Genome(int id, const std::vector<NodeGene> genes, const std::vector<ConnectionGene> connections) : 
    genomeID_{id},
    fitness_{0.0f},
    adjustedFitness_{0.0f},
    speciesID_{-1},
    nodes_{genes},
    connections_{connections}{}

void Genome::mutateWeights(std::mt19937& rng){
    std::uniform_real_distribution<float> distProb(0.0f, 1.0f);
    std::uniform_real_distribution<float> smlChng(-0.5f, 0.5f);
    std::uniform_real_distribution<float> bigChng(-2.0f, 2.0f);

    for (ConnectionGene& connection : connections_) {
        if (distProb(rng) < 0.8f) { 
            if (distProb(rng) < 0.9f) { 
                connection.weight += smlChng(rng);
            } 
            else { 
                connection.weight = bigChng(rng);
            }

        if (connection.weight > 8.0f) connection.weight = 8.0f;
        if (connection.weight < -8.0f) connection.weight = -8.0f;
        }
    }
}


void Genome::mutateAddConnection(std::mt19937& rng, InnovationManager& innovManager){
    if (nodes_.empty()) return;
    std::uniform_int_distribution<int> rngNode(0,nodes_.size()-1);
    int nodeAID = rngNode(rng);
    int nodeBID = rngNode(rng);
    NodeGene nodeA = nodes_[nodeAID];
    NodeGene nodeB = nodes_[nodeBID];

    if (nodeA.nodeID == nodeB.nodeID) return;
    for (const ConnectionGene& connection : connections_){
        if ((connection.inNodeID == nodeA.nodeID && connection.outNodeID == nodeB.nodeID )
            || (connection.inNodeID == nodeB.nodeID && connection.outNodeID == nodeA.nodeID)) return;
    }

    if (nodeA.layerX >= nodeB.layerX){
    std::swap(nodeAID, nodeBID);
    nodeA = nodes_[nodeAID];
    nodeB = nodes_[nodeBID];
}

    int innovID = innovManager.getInnovationID(nodes_[nodeAID].nodeID, nodes_[nodeBID].nodeID);
    std::uniform_real_distribution<float> newWeight(-1.0f,1.0f);
    ConnectionGene newConncetion(nodes_[nodeAID].nodeID,
                                 nodes_[nodeBID].nodeID,
                                newWeight(rng),
                                true,
                                innovID);
    connections_.push_back(newConncetion);
}

void Genome::mutateAddNode(std::mt19937& rng, InnovationManager& innovManager){
    if (connections_.empty()) return;
    std::uniform_int_distribution<int> rngConnect(0, connections_.size() -1 );
    int oldConnectionID = rngConnect(rng);
    ConnectionGene oldConnection = connections_[oldConnectionID];
    connections_[oldConnectionID].isEnabled = false;
    int newNodeID = innovManager.getNewNodeID();

    int oldInNodeID = oldConnection.inNodeID;
    int oldOutNodeID = oldConnection.outNodeID;

    auto itIN = std::find_if(nodes_.begin(), nodes_.end(), [oldInNodeID](const NodeGene& node) {return node.nodeID == oldInNodeID; });
    auto itOUT = std::find_if(nodes_.begin(), nodes_.end(), [oldOutNodeID](const NodeGene& node){ return node.nodeID == oldOutNodeID; });

    if (itIN != nodes_.end() && itOUT != nodes_.end()){
        float newLayerX = (itIN -> layerX + itOUT -> layerX) / 2.0f;
        NodeGene newNode(newNodeID, NodeType::HIDDEN, newLayerX);
        nodes_.push_back(newNode);

        ConnectionGene firstConnection(oldInNodeID, newNodeID, 1.0f, true, innovManager.getInnovationID(oldInNodeID,newNodeID));
        connections_.push_back(firstConnection);

        ConnectionGene secondConnection(newNodeID, oldOutNodeID, oldConnection.weight , true, innovManager.getInnovationID(newNodeID, oldOutNodeID));
        connections_.push_back(secondConnection);
    }
    return;
}

Genome Genome::crossover(int childID, const Genome& parent1, const Genome& parent2, std::mt19937& rng) {
    Genome child(childID);

    child.nodes_ = parent1.nodes_;

    std::uniform_real_distribution<float> distProb(0.0f, 1.0f);

    for (const ConnectionGene& gene1 : parent1.connections_) {
        ConnectionGene childGene = gene1;

        auto it = std::find_if(parent2.connections_.begin(), parent2.connections_.end(),
            [&gene1](const ConnectionGene& gene2) {
                return gene1.innovationID == gene2.innovationID;
            });

        if (it != parent2.connections_.end()) {
            if (distProb(rng) < 0.5f) {
                childGene.weight = it->weight;
            }

            if (!gene1.isEnabled || !it->isEnabled) {
                if (distProb(rng) < 0.75f) {
                    childGene.isEnabled = false;
                } else {
                    childGene.isEnabled = true;
                }
            }
        }

        child.connections_.push_back(childGene);
    }

    return child;
}



float Genome::getCompabilityDistance(const Genome& other, float c1, float c2, float c3) const {
    const auto& otherGenes = other.getConnections();
    int matching = 0;
    int disjoint = 0;
    int excess = 0;

    float weightDiffSum = 0.0f;
    int i = 0;
    int j = 0;

    float N = (connections_.size() >= otherGenes.size()) ? connections_.size() : otherGenes.size();
    if (N < 20.0f){
        N = 1.0f;
    }

    while (i < connections_.size() && j < otherGenes.size()){
        int inno1 = connections_[i].innovationID;
        int inno2 = otherGenes[j].innovationID;

        if (inno1 == inno2){
            float absVal = std::abs(connections_[i].weight - otherGenes[j].weight);
            weightDiffSum += absVal;
            matching++;
            i++;
            j++;
        } else if (inno1 < inno2){
            disjoint++;
            i++;
        } else {
            disjoint++;
            j++;
        }
    }

    excess += connections_.size() - i;
    excess += otherGenes.size() - j;

    float avgWeightDiff = (matching > 0) ? weightDiffSum / matching : 0.0f;
    float distance = (c1 * excess) / N + (c2 * disjoint) / N + c3 * avgWeightDiff;

    return distance;
}
