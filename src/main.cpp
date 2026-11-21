#include <raylib.h>

#include "rendering/TerrainRenderer.hpp"
#include "terrain/TerrainGenerator.hpp"
#include "gui/TerrainSettingsPanel.hpp"

int main()
{
    constexpr int screenWidth = 1080;
    constexpr int screenHeight = 1080;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(screenWidth, screenHeight, "MiniTG");
    SetTargetFPS(60);

    {
        TerrainSettings settings;
        TerrainSettingsPanel settingsPanel;

        TerrainMap map = generateTerrain(settings);
        TerrainRenderer renderer(map);

        constexpr float sidebarWidth = 320.0f;

        while (!WindowShouldClose())
        {
            BeginDrawing();
            ClearBackground(BLACK);

            const int currentWidth = GetRenderWidth();
            const int currentHeight = GetRenderHeight();

            const Rectangle sidebarBounds{
                static_cast<float>(currentWidth) - sidebarWidth,
                0.0f,
                sidebarWidth,
                static_cast<float>(currentHeight)
            };

            const bool generateRequested =
                settingsPanel.draw(settings, sidebarBounds);

            renderer.draw(currentWidth, currentHeight);

            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}
