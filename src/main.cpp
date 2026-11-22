#include <raylib.h>
#include <raygui.h>

#include "gui/TerrainSettingsPanel.hpp"
#include "rendering/TerrainRenderer.hpp"
#include "terrain/TerrainGenerator.hpp"

int main()
{
    constexpr int screenWidth = 1600;
    constexpr int screenHeight = 1200;
    constexpr int minimumTerrainWidth = 320;
    constexpr int minimumWindowHeight = 480;
    constexpr float sidebarWidth = 400.0f;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(screenWidth, screenHeight, "MiniTG");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    SetWindowMinSize(
        minimumTerrainWidth + static_cast<int>(sidebarWidth),
        minimumWindowHeight
    );
    SetTargetFPS(60);

    {
        TerrainSettings activeSettings;
        TerrainSettings draftSettings = activeSettings;
        TerrainSettingsPanel settingsPanel;

        TerrainMap map = generateTerrain(activeSettings);
        TerrainRenderer renderer(map);

        while (!WindowShouldClose())
        {
            BeginDrawing();
            ClearBackground(BLACK);

            const int currentWidth = GetRenderWidth();
            const int currentHeight = GetRenderHeight();

            const float terrainWidth =
                static_cast<float>(currentWidth) - sidebarWidth;

            const Rectangle terrainBounds{
                0.0f,
                0.0f,
                terrainWidth,
                static_cast<float>(currentHeight)
            };

            const Rectangle sidebarBounds{
                terrainWidth,
                0.0f,
                sidebarWidth,
                static_cast<float>(currentHeight)
            };

            renderer.draw(terrainBounds);

            // draw GUI last so it remains above rendered content
            const bool generateRequested = settingsPanel.draw(draftSettings, sidebarBounds);
            
            if (generateRequested) {
                activeSettings = draftSettings;
                // regenerate map and renderer here
            }

            EndDrawing();
        }
    }

    CloseWindow();
    return 0;
}
