#pragma once

#include "Spider.hpp"
#include "../genes/Genome.hpp"
#include <vector>
#include <box2d/box2d.h>


class PopulationManager{
private:
    std::vector<Spider> spiders_;
    b2WorldId worldID_;
    int populationSize_;
    int generation_;
    float timer_;
    const float generationTime_ = 15.0f;

    void startGeneration();
public:
    PopulationManager(int populSize, b2WorldId worldID);
    void update(float deltaTime);
    void draw() const;

    int getGeneration() const { return generation_; }
    float getTimer() const {return timer_; }
};