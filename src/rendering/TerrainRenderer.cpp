#include "TerrainRenderer.hpp"

#include <algorithm>

namespace {

Color terrainColor(TerrainType type) {
    switch (type) {
    case TerrainType::DeepWater:
        return Color{20, 55, 120, 255};
    case TerrainType::Water:
        return Color{35, 110, 180, 255};
    case TerrainType::Sand:
        return Color{220, 205, 140, 255};
    case TerrainType::Desert:
        return Color{205, 180, 105, 255};
    case TerrainType::Grass:
        return Color{70, 145, 75, 255};
    case TerrainType::Forest:
        return Color{35, 100, 50, 255};
    case TerrainType::Rock:
        return Color{115, 110, 105, 255};
    case TerrainType::Snow:
        return Color{240, 245, 250, 255};
    }

    return MAGENTA; // unexpected values are visually obvious
}

Color moistureColor(MoistureBand band) {
    switch (band) {
    case MoistureBand::Dry:
        return Color{170, 215, 240, 255};
    case MoistureBand::Moderate:
        return Color{65, 145, 210, 255};
    case MoistureBand::Wet:
        return Color{15, 55, 120, 255};
    }

    return MAGENTA; // unexpected values are visually obvious
}

} // namespace

TerrainRenderer::TerrainRenderer(const TerrainMap& map) {
    Image terrainImage = GenImageColor(map.width(), map.height(), BLACK);
    Image moistureImage = GenImageColor(map.width(), map.height(), BLACK);

    // convert every terrain cell into a color depending on the view type
    for (int y = 0; y < map.height(); y++) {
        for (int x = 0; x < map.width(); x++) {
            const TerrainCell& cell = map.at(x, y);

            // regular terrain view
            ImageDrawPixel(&terrainImage, x, y, terrainColor(cell.type));

            // moisture view
            ImageDrawPixel(
                &moistureImage,
                x,
                y,
                moistureColor(cell.moistureBand)
            );
        }
    }

    // upload images to gpu textures
    terrainTexture_ = LoadTextureFromImage(terrainImage);
    moistureTexture_ = LoadTextureFromImage(moistureImage);

    // release cpu images after upload
    UnloadImage(terrainImage);
    UnloadImage(moistureImage);

    SetTextureFilter(terrainTexture_, TEXTURE_FILTER_POINT);
    SetTextureFilter(moistureTexture_, TEXTURE_FILTER_POINT);
}

TerrainRenderer::~TerrainRenderer() {
    UnloadTexture(terrainTexture_);
    UnloadTexture(moistureTexture_);
}

void TerrainRenderer::draw(Rectangle viewport, TerrainView view) const {
    const Texture2D* texture = &terrainTexture_;
    if (view == TerrainView::Moisture) {
        texture = &moistureTexture_;
    }

    const float scale = std::min(
        viewport.width / texture->width,
        viewport.height / texture->height
    );

    const float drawWidth = texture->width * scale;
    const float drawHeight = texture->height * scale;

    const Rectangle source {
        0.0f,
        0.0f,
        static_cast<float>(texture->width),
        static_cast<float>(texture->height)
    };

    const Rectangle destination{
        viewport.x + (viewport.width - drawWidth) / 2.0f,
        viewport.y + (viewport.height - drawHeight) / 2.0f,
        drawWidth,
        drawHeight
    };

    DrawTexturePro(
        *texture,
        source,
        destination,
        Vector2{0.0f, 0.0f},
        0.0f,
        WHITE
    );
}
