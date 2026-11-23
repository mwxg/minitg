#pragma once

#include "TerrainMap.hpp"
#include "TerrainSettings.hpp"

#include <optional>
#include <stop_token>

TerrainMap generateTerrain(const TerrainSettings& settings);
std::optional<TerrainMap> generateTerrain(
    const TerrainSettings& settings,
    std::stop_token stopToken
);
