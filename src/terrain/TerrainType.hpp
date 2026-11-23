#pragma once

#include <cstdint>

enum class TerrainType : std::uint8_t {
    DeepWater,
    Water,
    FrozenWater,
    Sand,
    Desert,
    Savanna,
    Grass,
    Forest,
    Rainforest,
    Taiga,
    Tundra,
    Rock,
    Snow
};
