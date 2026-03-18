# SpiderML

SpiderML is a spider locomotion evolution simulation project built with:
- NEAT (NeuroEvolution of Augmenting Topologies)
- Box2D (physics)
- raylib (rendering and UI)

Each individual (spider) has its own genome and neural network that controls joint motors. Across generations, the population is evaluated by fitness, grouped into species, crossed over, and mutated.

## Training Plot

![Training analysis](data_analyse/analyse2.png)

## What This Project Does

- Simulates a population of spiders in a 2D physics world.
- Evolves neural controllers for leg movement.
- Displays training metrics on screen.
- Logs per-generation statistics to a CSV file.

## Requirements

- CMake 3.15+
- C++17 compiler
- raylib
- Box2D

The project uses find_package(... CONFIG ...), so raylib and box2d must be available as CMake config packages in your environment.

## Build And Run

From the project root:

    cmake -S . -B build
    cmake --build build -j
    ./build/SpiderML

## Where Statistics Are Stored

The CSV file is written to:

- build/training_stats.csv

Columns:

- generation
- best_fitness
- avg_fitness
- median_fitness
- stddev_fitness
- species_count
- rolling_best_10
- rolling_avg_10

Note: if the CSV header format changes, the application may recreate the file with the new header.

## Key NEAT Parameters

Parameters are defined in:

- src/evolution/NeatConfig.hpp

Most frequently tuned:

- kGenerationTimeSeconds: duration of one generation
- kCompatibilityThreshold: sensitivity of species splitting
- kCompatibilityC1, kCompatibilityC2, kCompatibilityC3: genome distance terms
- kGenomeMutateWeightsChance: chance of weight mutation per genome
- kAddConnectionChance: chance to add a connection
- kAddNodeChance: chance to add a node
- kInitialConnectionProbability: initial topology density
- kElitismCount: number of elites copied without mutation

## Current Simulation Setup

- Population size set in main.cpp: 200
- All spiders start from the same position (spawn spacing = 0)
- Generation duration: 20 s

## Metric Interpretation

- Increasing rolling_avg_10 means the whole population is improving.
- Increasing rolling_best_10 means top strategies are improving.
- High stddev_fitness means high spread in individual quality.
- species_count staying at 1 usually indicates weak divergence or overly permissive speciation.

## Directory Structure

- src/agent: spider body and simulation-level population management
- src/brain: neural network runtime
- src/genes: genome, population, mutations, crossover, speciation
- src/evolution: NEAT configuration
- build: build files and executable

## Typical Experiment Workflow

1. Set parameters in src/evolution/NeatConfig.hpp.
2. Build and run the project.
3. Let it run for dozens of generations.
4. Evaluate trends in build/training_stats.csv.
5. Adjust parameters and repeat.

## Further Ideas

- Add a headless mode for faster training (no rendering).
- Export the best genome and support replay.
- Add dynamic compatibility-threshold tuning to target a desired species count.
