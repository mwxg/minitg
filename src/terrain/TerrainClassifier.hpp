#include "TerrainSettings.hpp"
#include "TerrainType.hpp"

TerrainType classifyTerrain(
    float elevation,
    const TerrainThresholds& thresholds
);