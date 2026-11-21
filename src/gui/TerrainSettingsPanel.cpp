#include "TerrainSettingsPanel.hpp"

#include <raygui.h>

bool TerrainSettingsPanel::draw(TerrainSettings& draftSettings, Rectangle bounds) {
    GuiPanel(bounds, "Terrain Settings");

    constexpr float padding = 16.0f;
    constexpr float controlHeight = 28.0f;

    const float contentX = bounds.x + padding;
    const float contentWidth = bounds.width - padding * 2.0f;

    float currentY = bounds.y + 40.0f;

    GuiLabel(
        Rectangle{
            contentX,
            currentY,
            contentWidth,
            controlHeight
        },
        "Generation controls will go here"
    );

    currentY += controlHeight + 12.0f;

    const bool applyPressed = GuiButton(
        Rectangle{
            contentX,
            currentY,
            contentWidth,
            controlHeight
        },
        "Generate Terrain"
    );

    return applyPressed;
}