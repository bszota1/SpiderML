#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <raylib.h>

#include "PopulationManager.hpp"

namespace {
constexpr int kPopulationSize = 300;
constexpr float kFixedTimeStep = 1.0f / 60.0f;

void runWindowed() {
    InitWindow(1200, 800, "Spider Evolution NEAT");
    SetTargetFPS(60);

    PopulationManager popManager(kPopulationSize);

    while (!WindowShouldClose()) {
        popManager.update(GetFrameTime());

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangle(-100, 530, 4000, 40, DARKGRAY);
        
        popManager.draw();

        DrawText(TextFormat("Pokolenie: %i", popManager.getGeneration()), 20, 20, 20, BLACK);
        DrawText(TextFormat("Czas tury: %.1f / %.1f", popManager.getTimer(), popManager.getGenerationTime()), 20, 50, 20, DARKGRAY);
        DrawText(TextFormat("Best fitness: %.2f", popManager.getBestFitness()), 20, 80, 20, BLUE);
        DrawText(TextFormat("Avg fitness: %.2f", popManager.getAvgFitness()), 20, 110, 20, DARKBLUE);
        DrawText(TextFormat("Median fitness: %.2f", popManager.getMedianFitness()), 20, 140, 20, DARKGREEN);
        DrawText(TextFormat("Stddev fitness: %.2f", popManager.getStdDevFitness()), 20, 170, 20, DARKPURPLE);
        DrawText(TextFormat("Species: %i", popManager.getSpeciesCount()), 20, 200, 20, MAROON);
        DrawText(TextFormat("Rolling best (10): %.2f", popManager.getRollingBestFitness()), 20, 230, 20, PURPLE);
        DrawText(TextFormat("Rolling avg (10): %.2f", popManager.getRollingAvgFitness()), 20, 260, 20, VIOLET);

        EndDrawing();
    }

    CloseWindow();
}

void runHeadless(int maxGenerations) {
    PopulationManager popManager(kPopulationSize);

    using Clock = std::chrono::steady_clock;
    const auto start = Clock::now();
    int lastGeneration = popManager.getGeneration();

    while (maxGenerations <= 0 || popManager.getGeneration() - 1 < maxGenerations) {
        popManager.update(kFixedTimeStep);

        if (popManager.getGeneration() != lastGeneration) {
            lastGeneration = popManager.getGeneration();
            const double elapsed = std::chrono::duration<double>(Clock::now() - start).count();
            const int completed = lastGeneration - 1;
            std::cout << "Generation " << completed
                      << " | species: " << popManager.getSpeciesCount()
                      << " | rolling best (10): " << popManager.getRollingBestFitness()
                      << " | " << (completed / elapsed * 60.0) << " gen/min" << std::endl;
        }
    }

    const double elapsed = std::chrono::duration<double>(Clock::now() - start).count();
    const int completed = popManager.getGeneration() - 1;
    std::cout << "Completed " << completed << " generations in " << elapsed << " s ("
              << (completed / elapsed * 60.0) << " gen/min)\n";
}
}

int main(int argc, char** argv) {
    bool headless = false;
    int maxGenerations = 0;
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--headless") == 0) {
            headless = true;
        } else if (std::strcmp(argv[i], "--generations") == 0 && i + 1 < argc) {
            maxGenerations = std::atoi(argv[++i]);
        }
    }

    if (headless) {
        runHeadless(maxGenerations);
    } else {
        runWindowed();
    }

    return 0;
}
