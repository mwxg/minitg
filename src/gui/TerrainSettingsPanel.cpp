#include "TerrainSettingsPanel.hpp"

#include <raygui.h>

#include <algorithm>
#include <limits>

bool TerrainSettingsPanel::draw(TerrainSettings& draftSettings, Rectangle bounds) {
    GuiPanel(bounds, "Terrain Settings");

    constexpr float padding = 16.0f;
    constexpr float controlHeight = 28.0f;
    constexpr float rowSpacing = 8.0f;
    constexpr float labelWidth = 92.0f;
    constexpr int minimumMapSize = 64;
    constexpr int maximumMapSize = 2048;

    const float contentX = bounds.x + padding;
    const float contentWidth = bounds.width - padding * 2.0f;
    const float controlX = contentX + labelWidth;
    const float inputWidth = contentWidth - labelWidth;

    float currentY = bounds.y + 40.0f;

    const auto drawLabel = [&](const char* text) {
        GuiLabel(
            Rectangle{contentX, currentY, labelWidth, controlHeight},
            text
        );
    };

    const auto advanceRow = [&]() {
        currentY += controlHeight + rowSpacing;
    };

    drawLabel("Width");
    if (GuiSpinner(
            Rectangle{controlX, currentY, inputWidth, controlHeight},
            nullptr,
            &draftSettings.width,
            minimumMapSize,
            maximumMapSize,
            widthEditMode_)) {
        widthEditMode_ = !widthEditMode_;
    }
    advanceRow();

    drawLabel("Height");
    if (GuiSpinner(
            Rectangle{controlX, currentY, inputWidth, controlHeight},
            nullptr,
            &draftSettings.height,
            minimumMapSize,
            maximumMapSize,
            heightEditMode_)) {
        heightEditMode_ = !heightEditMode_;
    }
    advanceRow();

    int seedValue = static_cast<int>(std::min(
        draftSettings.seed,
        static_cast<unsigned int>(std::numeric_limits<int>::max())
    ));

    drawLabel("Seed");
    if (GuiValueBox(
            Rectangle{controlX, currentY, inputWidth, controlHeight},
            nullptr,
            &seedValue,
            0,
            std::numeric_limits<int>::max(),
            seedEditMode_)) {
        seedEditMode_ = !seedEditMode_;
    }
    draftSettings.seed = static_cast<unsigned int>(seedValue);
    advanceRow();

    drawLabel("Frequency");
    GuiSliderBar(
        Rectangle{controlX, currentY, inputWidth, controlHeight},
        nullptr,
        TextFormat("%.3f", draftSettings.frequency),
        &draftSettings.frequency,
        0.001f,
        0.1f
    );
    advanceRow();

    drawLabel("Octaves");
    if (GuiSpinner(
            Rectangle{controlX, currentY, inputWidth, controlHeight},
            nullptr,
            &draftSettings.octaves,
            1,
            10,
            octavesEditMode_)) {
        octavesEditMode_ = !octavesEditMode_;
    }
    advanceRow();

    drawLabel("Persistence");
    GuiSliderBar(
        Rectangle{controlX, currentY, inputWidth, controlHeight},
        nullptr,
        TextFormat("%.2f", draftSettings.persistence),
        &draftSettings.persistence,
        0.0f,
        1.0f
    );
    advanceRow();

    drawLabel("Lacunarity");
    GuiSliderBar(
        Rectangle{controlX, currentY, inputWidth, controlHeight},
        nullptr,
        TextFormat("%.2f", draftSettings.lacunarity),
        &draftSettings.lacunarity,
        1.0f,
        4.0f
    );
    advanceRow();

    currentY += 4.0f;

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
