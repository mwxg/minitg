#pragma once

#include <cstdint>

enum class TerrainType : std::uint8_t {
    DeepWater,
    Water,
    Sand,
    Desert,
    Grass,
    Forest,
    Rock,
    Snow
};
