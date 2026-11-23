#include "TerrainSettings.hpp"
#include "TerrainType.hpp"

TerrainType classifyTerrain(
    float elevation,
    float moisture,
    const TerrainThresholds& thresholds,
    const MoistureThresholds& moistureThresholds
);
