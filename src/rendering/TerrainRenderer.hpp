#pragma once

#include <raylib.h>

#include "terrain/TerrainMap.hpp"

class TerrainRenderer {
public:
    explicit TerrainRenderer(const TerrainMap& map);
    ~TerrainRenderer();

    // prevent multiple renderers from owning the same texture
    TerrainRenderer(const TerrainRenderer&) = delete;
    TerrainRenderer& operator=(const TerrainRenderer&) = delete;

    void draw(int screenWidth, int screenHeight) const;

private:
    // GPU texture owned by this renderer
    Texture2D texture_{};
};
