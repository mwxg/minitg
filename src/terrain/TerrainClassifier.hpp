#include "TerrainSettings.hpp"
#include "TerrainType.hpp"

[[nodiscard]] TerrainType classifyTerrain(
    float elevation,
    const TerrainThresholds& thresholds
);