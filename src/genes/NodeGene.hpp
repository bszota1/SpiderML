#pragma once 


enum class NodeType{
    INPUT,
    BIAS,
    HIDDEN,
    OUTPUT
};

struct NodeGene{
    int nodeID;
    NodeType nodeType;
    float layerX;

    NodeGene(int ID, NodeType type) : nodeID{ID}, nodeType{type}, layerX{0.0f}{}
    NodeGene(int ID, NodeType type, float layerX) : nodeID{ID}, nodeType{type}, layerX{layerX}{}
    NodeGene(const NodeGene& other) : nodeID{other.nodeID}, nodeType{other.nodeType}, layerX{other.layerX}{}
    bool operator==(const NodeGene& other) const { return this->nodeID == other.nodeID; }
};

const int INPUT_NODES = 37;
const int OUTPUT_NODES = 16;
const int TOTAL_NODES = INPUT_NODES + OUTPUT_NODES + 1;