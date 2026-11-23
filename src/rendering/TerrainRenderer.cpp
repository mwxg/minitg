#include "TerrainRenderer.hpp"

#include "../terrain/TerrainClassifier.hpp"

#include <algorithm>
#include <cstdint>

namespace {

enum class MoistureBand : std::uint8_t {
    Dry,
    Moderate,
    Wet
};

enum class ElevationBand : std::uint8_t {
    DeepWater,
    Water,
    Sand,
    Grass,
    Rock,
    Snow
};

Color terrainColor(TerrainType type) {
    switch (type) {
    case TerrainType::DeepWater:
        return Color{20, 55, 120, 255};
    case TerrainType::Water:
        return Color{35, 110, 180, 255};
    case TerrainType::FrozenWater:
        return Color{165, 205, 225, 255};
    case TerrainType::Sand:
        return Color{220, 205, 140, 255};
    case TerrainType::Desert:
        return Color{205, 180, 105, 255};
    case TerrainType::Savanna:
        return Color{165, 175, 75, 255};
    case TerrainType::Grass:
        return Color{70, 145, 75, 255};
    case TerrainType::Forest:
        return Color{25, 105, 70, 255};
    case TerrainType::Rainforest:
        return Color{10, 75, 35, 255};
    case TerrainType::Taiga:
        return Color{55, 100, 100, 255};
    case TerrainType::Tundra:
        return Color{150, 160, 135, 255};
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

Color elevationColor(ElevationBand band) {
    switch (band) {
    case ElevationBand::DeepWater:
        return Color{25, 25, 25, 255};
    case ElevationBand::Water:
        return Color{70, 70, 70, 255};
    case ElevationBand::Sand:
        return Color{115, 115, 115, 255};
    case ElevationBand::Grass:
        return Color{160, 160, 160, 255};
    case ElevationBand::Rock:
        return Color{205, 205, 205, 255};
    case ElevationBand::Snow:
        return Color{245, 245, 245, 255};
    }

    return MAGENTA; // unexpected values are visually obvious
}

Color temperatureColor(float temperature) {
    const float value = std::clamp(temperature, 0.0f, 1.0f);
    const auto red = static_cast<unsigned char>(255.0f * value);
    const auto blue = static_cast<unsigned char>(255.0f * (1.0f - value));

    return Color{red, 80, blue, 255};
}

MoistureBand classifyMoisture(
    float moisture,
    const MoistureThresholds& thresholds
) noexcept {
    if (moisture < thresholds.dry) {
        return MoistureBand::Dry;
    }
    if (moisture >= thresholds.wet) {
        return MoistureBand::Wet;
    }
    return MoistureBand::Moderate;
}

ElevationBand classifyElevation(
    float elevation,
    const TerrainThresholds& thresholds
) noexcept {
    if (elevation < thresholds.deepWater) {
        return ElevationBand::DeepWater;
    }
    if (elevation < thresholds.water) {
        return ElevationBand::Water;
    }
    if (elevation < thresholds.sand) {
        return ElevationBand::Sand;
    }
    if (elevation < thresholds.grass) {
        return ElevationBand::Grass;
    }
    if (elevation < thresholds.rock) {
        return ElevationBand::Rock;
    }
    return ElevationBand::Snow;
}

} // namespace

TerrainRenderer::TerrainRenderer(
    const TerrainMap& map,
    const TerrainSettings& settings
) {
    Image terrainImage = GenImageColor(map.width(), map.height(), BLACK);
    Image moistureImage = GenImageColor(map.width(), map.height(), BLACK);
    Image temperatureImage = GenImageColor(map.width(), map.height(), BLACK);
    Image elevationImage = GenImageColor(map.width(), map.height(), BLACK);

    // convert every terrain cell into a color depending on the view type
    for (int y = 0; y < map.height(); y++) {
        for (int x = 0; x < map.width(); x++) {
            const TerrainCell& cell = map.at(x, y);

            // regular terrain view
            const TerrainType type = classifyTerrain(
                cell.elevation,
                cell.moisture,
                cell.temperature,
                settings.thresholds,
                settings.moistureThresholds,
                settings.temperatureThresholds
            );
            ImageDrawPixel(&terrainImage, x, y, terrainColor(type));

            // moisture view
            ImageDrawPixel(
                &moistureImage,
                x,
                y,
                moistureColor(classifyMoisture(
                    cell.moisture,
                    settings.moistureThresholds
                ))
            );

            // temperature view
            ImageDrawPixel(
                &temperatureImage,
                x,
                y,
                temperatureColor(cell.temperature)
            );

            // elevation view
            ImageDrawPixel(
                &elevationImage,
                x,
                y,
                elevationColor(classifyElevation(
                    cell.elevation,
                    settings.thresholds
                ))
            );
        }
    }

    // upload images to gpu textures
    terrainTexture_ = LoadTextureFromImage(terrainImage);
    moistureTexture_ = LoadTextureFromImage(moistureImage);
    temperatureTexture_ = LoadTextureFromImage(temperatureImage);
    elevationTexture_ = LoadTextureFromImage(elevationImage);

    // release cpu images after upload
    UnloadImage(terrainImage);
    UnloadImage(moistureImage);
    UnloadImage(temperatureImage);
    UnloadImage(elevationImage);

    SetTextureFilter(terrainTexture_, TEXTURE_FILTER_POINT);
    SetTextureFilter(moistureTexture_, TEXTURE_FILTER_POINT);
    SetTextureFilter(temperatureTexture_, TEXTURE_FILTER_POINT);
    SetTextureFilter(elevationTexture_, TEXTURE_FILTER_POINT);
}

TerrainRenderer::~TerrainRenderer() {
    UnloadTexture(terrainTexture_);
    UnloadTexture(moistureTexture_);
    UnloadTexture(temperatureTexture_);
    UnloadTexture(elevationTexture_);
}

void TerrainRenderer::draw(Rectangle viewport, TerrainView view) const {
    const Texture2D* texture = &terrainTexture_;
    if (view == TerrainView::Moisture) {
        texture = &moistureTexture_;
    } else if (view == TerrainView::Temperature) {
        texture = &temperatureTexture_;
    } else if (view == TerrainView::Elevation) {
        texture = &elevationTexture_;
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
