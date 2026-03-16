#pragma once

#include "../genes/NodeGene.hpp"

struct Neuron
{
    NodeType type;
    float value;

    Neuron(NodeType type, float value) : type{type}, value{value}{}
};




