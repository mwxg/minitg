#pragma once

#include "TerrainSettings.hpp"

#include <vector>

struct TerrainCell {
    float elevation = 0.0f;
    float moisture = 0.0f;
    float temperature = 0.0f;
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
