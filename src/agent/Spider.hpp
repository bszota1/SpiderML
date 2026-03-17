#pragma once

#include "../brain/NeuralNetwork.hpp"
#include "../genes/Genome.hpp"
#include <raylib.h>
#include <box2d/box2d.h>
#include <vector>

class Spider{
private:
    Genome genome_;
    NeuralNetwork neuralNetwork_;
    b2WorldId worldId_;
    b2BodyId torso_;
    std::vector<b2BodyId> legs_;
    std::vector<b2JointId> joints_;
    Vector2 bodyPosition_;
    bool isAlive_;
    float fitness_;

    void buildBody(const Vector2& startPosition);
public:
    Spider(const Genome& genome, const Vector2& position, b2WorldId worldId);
    ~Spider();
    void update();
    void draw() const;

    Genome getGenome() const { return genome_; }
    bool getIsAlive() const { return isAlive_; }
    float getFitness() const { return fitness_; }

    void kill();
};