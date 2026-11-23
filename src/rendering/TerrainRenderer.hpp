#pragma once

#include <raylib.h>

#include "../terrain/TerrainMap.hpp"
#include "TerrainView.hpp"

class TerrainRenderer {
public:
    TerrainRenderer(const TerrainMap& map, const TerrainSettings& settings);
    ~TerrainRenderer();

    // prevent multiple renderers from owning the same texture
    TerrainRenderer(const TerrainRenderer&) = delete;
    TerrainRenderer& operator=(const TerrainRenderer&) = delete;

    void draw(Rectangle viewport, TerrainView view) const;

private:
    // gpu textures owned by this renderer
    Texture2D terrainTexture_{};
    Texture2D moistureTexture_{};
    Texture2D temperatureTexture_{};
};
