#include <raylib.h>
#include <raygui.h>

#include <filesystem>
#include <chrono>
#include <future>
#include <optional>
#include <string>
#include <memory>

#include "gui/TerrainSettingsPanel.hpp"
#include "rendering/TerrainRenderer.hpp"
#include "rendering/TerrainView.hpp"
#include "terrain/TerrainGenerator.hpp"

int main() {
    constexpr int screenWidth = 1600;
    constexpr int screenHeight = 1200;
    constexpr int minimumTerrainWidth = 320;
    constexpr int minimumWindowHeight = 480;
    constexpr float sidebarWidth = 500.0f;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(screenWidth, screenHeight, "MiniTG");
    SetWindowMinSize(
        minimumTerrainWidth + static_cast<int>(sidebarWidth),
        minimumWindowHeight
    );
    SetTargetFPS(60);

    const std::filesystem::path fontPath =
        std::filesystem::path(GetApplicationDirectory()) /
        "assets" / "fonts" / "Quantico-Regular.ttf";

    Font uiFont = LoadFontEx(
        fontPath.string().c_str(),
        64,
        nullptr,
        0
    );

    if (IsFontValid(uiFont)) {
        GuiSetFont(uiFont);
    } else {
        TraceLog(LOG_WARNING, "Failed to load UI font: %s", fontPath.string().c_str());
    }

    GuiSetStyle(DEFAULT, TEXT_SIZE, 28);

    // green colors for the gui
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, 0x3F8F5FFF);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0xCDEBD7FF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0x245C3AFF);
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, 0x287A48FF);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x8FD3A8FF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, 0x174A2DFF);

    { // keep rendering resources scoped so they are destroyed before CloseWindow()
        TerrainSettings activeSettings;
        TerrainSettings draftSettings = activeSettings;
        TerrainSettingsPanel settingsPanel;

        TerrainMap map = generateTerrain(activeSettings);
        auto renderer = std::make_unique<TerrainRenderer>(map);

        std::future<TerrainMap> generationFuture;
        std::optional<TerrainSettings> pendingSettings;
        std::string generationError;

        TerrainView terrainView = TerrainView::Terrain;

        while (!WindowShouldClose()) {
            // poll for completion without blocking
            if (
                generationFuture.valid() &&
                generationFuture.wait_for(std::chrono::milliseconds{0}) ==
                std::future_status::ready
            ) {
                try {
                    // retrieve the completed map
                    TerrainMap newMap = generationFuture.get();

                    // create GPU resources on the main thread
                    auto newRenderer = std::make_unique<TerrainRenderer>(newMap);

                    // install the completed terrain state
                    map = std::move(newMap);
                    renderer = std::move(newRenderer);

                    activeSettings = pendingSettings.value();
                    pendingSettings.reset();
                } catch (const std::exception& error) {
                    generationError = error.what();
                    pendingSettings.reset();
                }
            }

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

            // draw the current terrain while generation runs
            renderer->draw(terrainBounds, terrainView);

            // draw GUI last so it remains above rendered content
            const bool generateRequested = settingsPanel.draw(
                draftSettings,
                terrainView,
                sidebarBounds,
                generationError
            );
            
            if (generateRequested && !generationFuture.valid()) {
                // snapshot the requested settings
                const TerrainSettings requestedSettings = draftSettings;
                pendingSettings = requestedSettings;

                generationError.clear();

                // launch generation in the background
                generationFuture = std::async(
                    std::launch::async,
                    [requestedSettings]() {
                        return generateTerrain(requestedSettings);
                    }
                );
            }

            EndDrawing();
        }
    }

    if (IsFontValid(uiFont)) {
        UnloadFont(uiFont);
    }

    CloseWindow();
    return 0;
}
