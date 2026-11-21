#include "TerrainRenderer.hpp"

#include <algorithm>

namespace {

Color terrainColor(TerrainType type)
{
    switch (type) {
    case TerrainType::DeepWater:
        return Color{20, 55, 120, 255};
    case TerrainType::Water:
        return Color{35, 110, 180, 255};
    case TerrainType::Sand:
        return Color{220, 205, 140, 255};
    case TerrainType::Grass:
        return Color{70, 145, 75, 255};
    case TerrainType::Rock:
        return Color{115, 110, 105, 255};
    case TerrainType::Snow:
        return Color{240, 245, 250, 255};
    }

    return MAGENTA; // unexpected values are visually obvious
}

} // namespace

TerrainRenderer::TerrainRenderer(const TerrainMap& map)
{
    Image image = GenImageColor(map.width(), map.height(), BLACK);

    // convert every terrain cell into a color
    for (int y = 0; y < map.height(); y++) {
        for (int x = 0; x < map.width(); x++) {
            const TerrainCell& cell = map.at(x, y);
            ImageDrawPixel(&image, x, y, terrainColor(cell.type));
        }
    }

    // Texture2D is uploaded to graphics memory while
    // Image is CPU-accessible
    texture_ = LoadTextureFromImage(image);

    // once data is in a texture, CPU-side image is no longer needed
    UnloadImage(image);

    SetTextureFilter(texture_, TEXTURE_FILTER_POINT);
}

TerrainRenderer::~TerrainRenderer()
{
    UnloadTexture(texture_);
}

void TerrainRenderer::draw(int screenWidth, int screenHeight) const
{
    const float scale = std::min(
        static_cast<float>(screenWidth) / texture_.width,
        static_cast<float>(screenHeight) / texture_.height
    );

    const float drawWidth = texture_.width * scale;
    const float drawHeight = texture_.height * scale;

    const Rectangle source {
        0.0f,
        0.0f,
        static_cast<float>(texture_.width),
        static_cast<float>(texture_.height)
    };

    const Rectangle destination{
        (screenWidth - drawWidth) / 2.0f,
        (screenHeight - drawHeight) / 2.0f,
        drawWidth,
        drawHeight
    };

    DrawTexturePro(
        texture_,
        source,
        destination,
        Vector2{0.0f, 0.0f},
        0.0f,
        WHITE
    );
}
