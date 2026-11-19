#pragma once

struct TerrainThresholds {
    float deepWater = 0.25f;
    float water = 0.40;
    float sand = 0.45f;
    float grass = 0.70f;
    float rock = 0.85f;
};

struct TerrainSettings {
    int width = 256;
    int height = 256;
    unsigned int seed = 1;

    TerrainThresholds thresholds;
};