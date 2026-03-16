#pragma once

struct Synapse
{
    int inNeuronID;
    int outNeuronID;
    float weight;

    Synapse(int inID, int outID, float weight) : inNeuronID{inID}, outNeuronID{outID}, weight{weight}{}

};
