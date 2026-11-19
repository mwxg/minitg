#include <raylib.h>
#include <algorithm>

#include "terrain/TerrainGenerator.hpp"

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

int main()
{
    constexpr int screenWidth = 1080;
    constexpr int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "MiniTG");
    SetTargetFPS(60);

    TerrainSettings settings;
    TerrainMap map = generateTerrain(settings);

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
    Texture2D texture = LoadTextureFromImage(image);

    // once data is in a texture, CPU-side image is no longer needed
    UnloadImage(image);

    SetTextureFilter(texture, TEXTURE_FILTER_POINT);

    const float scale = std::min(
        static_cast<float>(screenWidth) / texture.width,
        static_cast<float>(screenHeight) / texture.height
    );

    const float drawWidth = texture.width * scale;
    const float drawHeight = texture.height * scale;

    const Rectangle source {
        0.0f,
        0.0f,
        static_cast<float>(texture.width),
        static_cast<float>(texture.height)
    };

    const Rectangle destination{
        (screenWidth - drawWidth) / 2.0f,
        (screenHeight - drawHeight) / 2.0f,
        drawWidth,
        drawHeight
    };

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(
            texture,
            source,
            destination,
            Vector2{0.0f, 0.0f},
            0.0f,
            WHITE
        );

        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();
    return 0;
}
