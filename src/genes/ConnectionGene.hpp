#pragma once

struct ConnectionGene
{
    int inNodeID;
    int outNodeID;
    float weight;
    bool isEnabled;
    int innovationID;

    ConnectionGene(int inID, int outID, float weight, bool isEnabled, int innovID) :
        inNodeID{inID},
        outNodeID{outID},
        weight{weight},
        isEnabled{isEnabled},
        innovationID{innovID}{}

    ConnectionGene(const ConnectionGene& other) = default;
};
