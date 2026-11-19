#include "TerrainClassifier.hpp"

TerrainType classifyTerrain(
    float elevation,
    const TerrainThresholds& thresholds
) {
    if (elevation < thresholds.deepWater) {
        return TerrainType::DeepWater;
    }

    if (elevation < thresholds.water) {
        return TerrainType::Water;
    }

    if (elevation < thresholds.sand) {
        return TerrainType::Sand;
    }

    if (elevation < thresholds.grass) {
        return TerrainType::Grass;
    }

    if (elevation < thresholds.rock) {
        return TerrainType::Rock;
    }

    return TerrainType::Snow;
}