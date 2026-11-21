#include <raylib.h>

#include "rendering/TerrainRenderer.hpp"
#include "terrain/TerrainGenerator.hpp"

int main()
{
    constexpr int screenWidth = 1080;
    constexpr int screenHeight = 1080;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(screenWidth, screenHeight, "MiniTG");
    SetTargetFPS(60);

    {
        TerrainSettings settings;
        TerrainMap map = generateTerrain(settings);
        TerrainRenderer renderer(map);

        while (!WindowShouldClose())
        {
            BeginDrawing();
            ClearBackground(BLACK);

            const int currentWidth = GetRenderWidth();
            const int currentHeight = GetRenderHeight();

            renderer.draw(currentWidth, currentHeight);

            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}
