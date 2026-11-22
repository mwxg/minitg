#pragma once

struct TerrainThresholds {
    float deepWater = 0.25f;
    float water = 0.40;
    float sand = 0.45f;
    float grass = 0.70f;
    float rock = 0.85f;
};

struct TerrainSettings {
    int width = 512;
    int height = 512;
    unsigned int seed = 1;

    // size of terrain features, lower means larger continents
    float frequency = 0.015f;

    // number of noise layers
    int octaves = 5;

    // how strongly smaller features contribute
    float persistence = 0.5f;

    // how quickly each layer becomes more detailed
    float lacunarity = 2.0f;

    bool islandFalloffEnabled = true;

    // where elevation reduction begins
    float islandRadius = 0.65f;

    // maximum amount removed from elevation
    float islandFalloffStrength = 0.6f;

    TerrainThresholds thresholds;
};