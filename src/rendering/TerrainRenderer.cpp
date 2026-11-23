#include "TerrainRenderer.hpp"

#include "../terrain/TerrainClassifier.hpp"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

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

class ImageOwner {
public:
    explicit ImageOwner(Image image) : image_(image) {}

    ~ImageOwner() {
        if (IsImageValid(image_)) {
            UnloadImage(image_);
        }
    }

    ImageOwner(const ImageOwner&) = delete;
    ImageOwner& operator=(const ImageOwner&) = delete;

    ImageOwner(ImageOwner&& other) noexcept
        : image_(std::exchange(other.image_, {})) {}

    Image& get() noexcept {
        return image_;
    }

private:
    Image image_{};
};

class TextureOwner {
public:
    explicit TextureOwner(Texture2D texture) : texture_(texture) {}

    ~TextureOwner() {
        if (IsTextureValid(texture_)) {
            UnloadTexture(texture_);
        }
    }

    TextureOwner(const TextureOwner&) = delete;
    TextureOwner& operator=(const TextureOwner&) = delete;

    TextureOwner(TextureOwner&& other) noexcept
        : texture_(std::exchange(other.texture_, {})) {}

    Texture2D release() noexcept {
        return std::exchange(texture_, {});
    }

private:
    Texture2D texture_{};
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

template <typename ColorFunction>
TextureOwner createTexture(
    const TerrainMap& map,
    const char* viewName,
    ColorFunction&& colorForCell
) {
    Image image = GenImageColor(map.width(), map.height(), BLACK);
    if (!IsImageValid(image)) {
        throw std::runtime_error(
            std::string{"Failed to allocate "} + viewName + " image"
        );
    }

    ImageOwner imageOwner{image};
    for (int y = 0; y < map.height(); y++) {
        for (int x = 0; x < map.width(); x++) {
            ImageDrawPixel(
                &imageOwner.get(),
                x,
                y,
                colorForCell(map.at(x, y))
            );
        }
    }

    Texture2D texture = LoadTextureFromImage(imageOwner.get());
    if (!IsTextureValid(texture)) {
        throw std::runtime_error(
            std::string{"Failed to upload "} + viewName + " texture"
        );
    }

    SetTextureFilter(texture, TEXTURE_FILTER_POINT);
    return TextureOwner{texture};
}

} // namespace

TerrainRenderer::TerrainRenderer(
    const TerrainMap& map,
    const TerrainSettings& settings
) {
    TextureOwner terrain = createTexture(
        map,
        "terrain",
        [&settings](const TerrainCell& cell) {
            return terrainColor(classifyTerrain(
                cell.elevation,
                cell.moisture,
                cell.temperature,
                settings.thresholds,
                settings.moistureThresholds,
                settings.temperatureThresholds
            ));
        }
    );
    TextureOwner moisture = createTexture(
        map,
        "moisture",
        [&settings](const TerrainCell& cell) {
            return moistureColor(classifyMoisture(
                cell.moisture,
                settings.moistureThresholds
            ));
        }
    );
    TextureOwner temperature = createTexture(
        map,
        "temperature",
        [](const TerrainCell& cell) {
            return temperatureColor(cell.temperature);
        }
    );
    TextureOwner elevation = createTexture(
        map,
        "elevation",
        [&settings](const TerrainCell& cell) {
            return elevationColor(classifyElevation(
                cell.elevation,
                settings.thresholds
            ));
        }
    );

    terrainTexture_ = terrain.release();
    moistureTexture_ = moisture.release();
    temperatureTexture_ = temperature.release();
    elevationTexture_ = elevation.release();
}

TerrainRenderer::~TerrainRenderer() {
    if (IsTextureValid(terrainTexture_)) {
        UnloadTexture(terrainTexture_);
    }
    if (IsTextureValid(moistureTexture_)) {
        UnloadTexture(moistureTexture_);
    }
    if (IsTextureValid(temperatureTexture_)) {
        UnloadTexture(temperatureTexture_);
    }
    if (IsTextureValid(elevationTexture_)) {
        UnloadTexture(elevationTexture_);
    }
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

    if (!IsTextureValid(*texture) ||
        texture->width <= 0 ||
        texture->height <= 0) {
        return;
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
