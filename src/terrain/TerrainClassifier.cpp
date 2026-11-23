#include "TerrainClassifier.hpp"

TerrainType classifyTerrain(
    float elevation,
    float moisture,
    float temperature,
    const TerrainThresholds& thresholds,
    const MoistureThresholds& moistureThresholds,
    const TemperatureThresholds& temperatureThresholds
) {
    const bool isCold = temperature < temperatureThresholds.cold;

    if (elevation < thresholds.deepWater) {
        return isCold ? TerrainType::FrozenWater : TerrainType::DeepWater;
    }

    if (elevation < thresholds.water) {
        return isCold ? TerrainType::FrozenWater : TerrainType::Water;
    }

    if (elevation < thresholds.sand) {
        return TerrainType::Sand;
    }

    if (elevation >= thresholds.rock) {
        return TerrainType::Snow;
    }

    if (elevation >= thresholds.grass) {
        return isCold ? TerrainType::Snow : TerrainType::Rock;
    }

    const bool isDry = moisture < moistureThresholds.dry;
    const bool isWet = moisture >= moistureThresholds.wet;

    if (isCold) {
        return isDry ? TerrainType::Tundra : TerrainType::Taiga;
    }

    if (temperature >= temperatureThresholds.hot) {
        if (isDry) {
            return TerrainType::Desert;
        }

        return isWet ? TerrainType::Rainforest : TerrainType::Savanna;
    }

    return isWet ? TerrainType::Forest : TerrainType::Grass;
}
