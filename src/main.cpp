#include <raylib.h>

#include "rendering/TerrainRenderer.hpp"
#include "terrain/TerrainGenerator.hpp"

int main()
{
    constexpr int screenWidth = 1080;
    constexpr int screenHeight = 1080;

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

            renderer.draw(screenWidth, screenHeight);

            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}
