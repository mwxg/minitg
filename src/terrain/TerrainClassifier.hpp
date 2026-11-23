#pragma once

#include "TerrainSettings.hpp"
#include "TerrainType.hpp"

TerrainType classifyTerrain(
    float elevation,
    float moisture,
    float temperature,
    const TerrainThresholds& thresholds,
    const MoistureThresholds& moistureThresholds,
    const TemperatureThresholds& temperatureThresholds
);
