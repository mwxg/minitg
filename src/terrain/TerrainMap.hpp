#pragma once

#include "TerrainType.hpp"
#include "TerrainSettings.hpp"

#include <vector>

enum class MoistureBand {
    Dry,
    Moderate,
    Wet
};

struct TerrainCell {
    float elevation = 0.0f;
    float moisture = 0.0f;
    MoistureBand moistureBand = MoistureBand::Dry;
    TerrainType type = TerrainType::DeepWater;
};

class TerrainMap {
public:
    explicit TerrainMap(const TerrainSettings& settings);
    int width() const noexcept;
    int height() const noexcept;

    TerrainCell& at(int x, int y);
    const TerrainCell& at(int x, int y) const;

private:
    int width_;
    int height_;
    std::vector<TerrainCell> cells_;
};
