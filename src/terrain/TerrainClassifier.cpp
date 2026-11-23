#include "TerrainClassifier.hpp"

TerrainType classifyTerrain(
    float elevation,
    float moisture,
    const TerrainThresholds& thresholds,
    const MoistureThresholds& moistureThresholds
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
        if (moisture < moistureThresholds.dry) {
            return TerrainType::Desert;
        }

        if (moisture >= moistureThresholds.wet) {
            return TerrainType::Forest;
        }

        return TerrainType::Grass;
    }

    if (elevation < thresholds.rock) {
        return TerrainType::Rock;
    }

    return TerrainType::Snow;
}
