#pragma once

namespace NeatConfig {
constexpr float kGenerationTimeSeconds = 20.0f;

constexpr float kSpawnBaseX = 200.0f;
constexpr float kSpawnSpacingX = 0.0f;
constexpr float kSpawnY = 400.0f;

constexpr float kCompatibilityC1 = 1.0f;
constexpr float kCompatibilityC2 = 1.0f;
constexpr float kCompatibilityC3 = 0.9f;
constexpr float kCompatibilityThreshold = 0.8f;
constexpr float kCompatibilityNormalizationCap = 20.0f;

constexpr float kWeightMutateConnectionChance = 0.25f;
constexpr float kWeightMutatePerturbChance = 0.90f;
constexpr float kWeightMutateSmallDeltaMin = -0.20f;
constexpr float kWeightMutateSmallDeltaMax = 0.20f;
constexpr float kWeightMutateResetMin = -2.0f;
constexpr float kWeightMutateResetMax = 2.0f;
constexpr float kMaxAbsWeight = 8.0f;

constexpr float kGenomeMutateWeightsChance = 0.25f;
constexpr float kAddConnectionChance = 0.08f;
constexpr float kAddNodeChance = 0.03f;
constexpr float kInitialConnectionProbability = 0.25f;

constexpr int kRollingWindowGenerations = 10;
constexpr int kElitismCount = 2;
}
