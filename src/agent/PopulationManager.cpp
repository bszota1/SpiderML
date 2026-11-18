#include "PopulationManager.hpp"
#include "../evolution/NeatConfig.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>

PopulationManager::PopulationManager(int populSize) : 
    populationSize_{populSize},
    generation_{1},
    timer_{0.0f},
    simTime_{0.0f},
    generationTime_{NeatConfig::kGenerationTimeSeconds},
    population_{populSize}{
        rng_ = std::mt19937(std::random_device{}());
        bestFitness_ = 0.0f;
        avgFitness_ = 0.0f;
        medianFitness_ = 0.0f;
        stdDevFitness_ = 0.0f;
        speciesCount_ = 0;
        rollingBestFitness_ = 0.0f;
        rollingAvgFitness_ = 0.0f;
        initStatsFile();
        createWorlds();
        createInitialPopulation();
        population_.speciate();
        speciesCount_ = population_.getSpeciesCount();
        updateFitnessStats();
    }

PopulationManager::~PopulationManager() {
    if (statsFile_.is_open()) {
        statsFile_.flush();
        statsFile_.close();
    }
    spiders_.clear();
    for (b2WorldId world : worlds_) {
        b2DestroyWorld(world);
    }
}

void PopulationManager::createWorlds() {
    const int worldCount = (populationSize_ + NeatConfig::kSpidersPerWorld - 1) / NeatConfig::kSpidersPerWorld;
    worlds_.reserve(worldCount);

    for (int i = 0; i < worldCount; i++) {
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = {0.0f, NeatConfig::kPhysicsGravityY};
        b2WorldId world = b2CreateWorld(&worldDef);

        b2BodyDef groundBodyDef = b2DefaultBodyDef();
        groundBodyDef.position = {1000.0f, 550.0f};
        b2BodyId groundId = b2CreateBody(world, &groundBodyDef);

        b2Polygon groundBox = b2MakeBox(2000.0f, 20.0f);
        b2ShapeDef groundShapeDef = b2DefaultShapeDef();
        groundShapeDef.material.friction = 0.8f;
        b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

        worlds_.push_back(world);
    }
}

void PopulationManager::spawnSpiders(const std::vector<Genome>& genomes) {
    spiders_.clear();
    spiders_.reserve(populationSize_);

    for (int i = 0; i < populationSize_ && i < static_cast<int>(genomes.size()); i++) {
        Vector2 startPosition = {
            NeatConfig::kSpawnBaseX + (i * NeatConfig::kSpawnSpacingX),
            NeatConfig::kSpawnY
        };
        spiders_.emplace_back(genomes[i], startPosition, worlds_[i / NeatConfig::kSpidersPerWorld]);
    }
}

void PopulationManager::initStatsFile() {
    const std::string statsPath = "training_stats.csv";
    const std::string expectedHeader = "generation,best_fitness,avg_fitness,median_fitness,stddev_fitness,species_count,rolling_best_10,rolling_avg_10";
    bool recreateFile = false;
    std::ifstream inFile(statsPath);

    if (inFile.good()) {
        std::string firstLine;
        if (std::getline(inFile, firstLine)) {
            if (!firstLine.empty() && firstLine != expectedHeader) {
                recreateFile = true;
            }
        }
    }

    if (recreateFile) {
        std::ofstream resetFile(statsPath, std::ios::trunc);
        if (resetFile.is_open()) {
            resetFile << expectedHeader << "\n";
            resetFile.close();
        }
    }

    statsFile_.open(statsPath, std::ios::app);
    if (!statsFile_.is_open()) {
        return;
    }

    std::ifstream verifyFile(statsPath);
    if (verifyFile.good()) {
        std::string firstLine;
        if (std::getline(verifyFile, firstLine)) {
            if (firstLine.empty()) {
                statsFile_ << expectedHeader << "\n";
                statsFile_.flush();
            }
        } else {
            statsFile_ << expectedHeader << "\n";
            statsFile_.flush();
        }
    }
}

void PopulationManager::updateFitnessStats() {
    if (spiders_.empty()) {
        bestFitness_ = 0.0f;
        avgFitness_ = 0.0f;
        medianFitness_ = 0.0f;
        stdDevFitness_ = 0.0f;
        return;
    }

    float best = spiders_.front().getFitness();
    float sum = 0.0f;
    std::vector<float> fitnessValues;
    fitnessValues.reserve(spiders_.size());

    for (const auto& spider : spiders_) {
        const float f = spider.getFitness();
        best = std::max(best, f);
        sum += f;
        fitnessValues.push_back(f);
    }

    std::sort(fitnessValues.begin(), fitnessValues.end());
    const size_t mid = fitnessValues.size() / 2;
    if (fitnessValues.size() % 2 == 0) {
        medianFitness_ = 0.5f * (fitnessValues[mid - 1] + fitnessValues[mid]);
    } else {
        medianFitness_ = fitnessValues[mid];
    }

    bestFitness_ = best;
    avgFitness_ = sum / static_cast<float>(spiders_.size());

    float variance = 0.0f;
    for (float value : fitnessValues) {
        float diff = value - avgFitness_;
        variance += diff * diff;
    }
    variance /= static_cast<float>(fitnessValues.size());
    stdDevFitness_ = std::sqrt(variance);
}

void PopulationManager::updateRollingMeans() {
    recentBestFitness_.push_back(bestFitness_);
    recentAvgFitness_.push_back(avgFitness_);

    while (static_cast<int>(recentBestFitness_.size()) > NeatConfig::kRollingWindowGenerations) {
        recentBestFitness_.pop_front();
    }
    while (static_cast<int>(recentAvgFitness_.size()) > NeatConfig::kRollingWindowGenerations) {
        recentAvgFitness_.pop_front();
    }

    float bestSum = 0.0f;
    for (float value : recentBestFitness_) {
        bestSum += value;
    }
    float avgSum = 0.0f;
    for (float value : recentAvgFitness_) {
        avgSum += value;
    }

    rollingBestFitness_ = recentBestFitness_.empty() ? 0.0f : bestSum / static_cast<float>(recentBestFitness_.size());
    rollingAvgFitness_ = recentAvgFitness_.empty() ? 0.0f : avgSum / static_cast<float>(recentAvgFitness_.size());
}

void PopulationManager::writeGenerationStatsCsv() {
    if (!statsFile_.is_open()) {
        return;
    }

    updateRollingMeans();
    statsFile_ << generation_ << "," << bestFitness_ << "," << avgFitness_ << "," << medianFitness_ << "," << stdDevFitness_ << "," << speciesCount_ << "," << rollingBestFitness_ << "," << rollingAvgFitness_ << "\n";
    statsFile_.flush();
}

void PopulationManager::createInitialPopulation() {
    spawnSpiders(population_.getPopulation());
}

void PopulationManager::nextGeneration() {
    auto& genomes = population_.getPopulationMutable();
    const int count = std::min(static_cast<int>(spiders_.size()), static_cast<int>(genomes.size()));
    for (int i = 0; i < count; i++) {
        genomes[i].fitness_ = spiders_[i].getFitness();
    }

    population_.speciate();
    speciesCount_ = population_.getSpeciesCount();
    population_.calculateFitness();
    writeGenerationStatsCsv();
    population_.nextGeneration();

    spawnSpiders(population_.getPopulation());

    updateFitnessStats();
}

void PopulationManager::update(float deltaTime){
    for (b2WorldId world : worlds_) {
        b2World_Step(world, deltaTime, NeatConfig::kPhysicsSubSteps);
    }

    timer_ += deltaTime;
    simTime_ += deltaTime;
    for (auto& spider : spiders_){
        spider.update(simTime_);
    }
    updateFitnessStats();

    if (timer_ >= generationTime_){
        nextGeneration();
        generation_++;
        timer_ = 0.0f;
    }
}

void PopulationManager::draw() const {
    for (const auto& spider : spiders_){
        spider.draw();
    }
}