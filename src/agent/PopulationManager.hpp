#pragma once

#include "Spider.hpp"
#include "../genes/Genome.hpp"
#include "../genes/InnovationManager.hpp"
#include "../genes/Population.hpp"
#include <vector>
#include <box2d/box2d.h>
#include <random>
#include <fstream>
#include <deque>


class PopulationManager{
private:
    std::vector<Spider> spiders_;
    b2WorldId worldID_;
    int populationSize_;
    int generation_;
    float timer_;
    float generationTime_;
    std::mt19937 rng_;
    InnovationManager innoManager_;
    Population population_;
    float bestFitness_;
    float avgFitness_;
    float medianFitness_;
    float stdDevFitness_;
    int speciesCount_;
    float rollingBestFitness_;
    float rollingAvgFitness_;
    std::deque<float> recentBestFitness_;
    std::deque<float> recentAvgFitness_;
    std::ofstream statsFile_;

    void updateFitnessStats();
    void updateRollingMeans();
    void initStatsFile();
    void writeGenerationStatsCsv();

    void nextGeneration();
public:
    PopulationManager(int populSize, b2WorldId worldID);
    ~PopulationManager();
    void update(float deltaTime);
    void draw() const;
    void createInitialPopulation();

    int getGeneration() const { return generation_; }
    float getTimer() const {return timer_; }
    float getBestFitness() const { return bestFitness_; }
    float getAvgFitness() const { return avgFitness_; }
    float getMedianFitness() const { return medianFitness_; }
    float getStdDevFitness() const { return stdDevFitness_; }
    int getSpeciesCount() const { return speciesCount_; }
    float getRollingBestFitness() const { return rollingBestFitness_; }
    float getRollingAvgFitness() const { return rollingAvgFitness_; }
    float getGenerationTime() const { return generationTime_; }
};