#pragma once

#include <cstdint>

struct NoiseSettings {
    std::uint32_t seed = 429195;
    float frequency = 0.015f;
    int octaves = 5;
    float persistence = 0.5f;
    float lacunarity = 2.0f;
};

struct TerrainThresholds {
    float deepWater = 0.25f;
    float water = 0.40;
    float sand = 0.45f;
    float grass = 0.70f;
    float rock = 0.85f;
};

struct MoistureThresholds {
    float dry = 0.35f;
    float wet = 0.65f;
};

struct TerrainSettings {
    int width = 512;
    int height = 512;
    NoiseSettings elevationNoise;

    // broader moisture regions
    NoiseSettings moistureNoise{
        0,
        0.008f,
        4,
        0.5f,
        2.0f
    };

    bool islandFalloffEnabled = true;

    // where elevation reduction begins
    float islandRadius = 0.65f;

    // maximum amount removed from elevation
    float islandFalloffStrength = 0.6f;

    TerrainThresholds thresholds;
    MoistureThresholds moistureThresholds;
};
