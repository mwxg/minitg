#include "TerrainSettingsPanel.hpp"

#include <raygui.h>

#include <algorithm>
#include <limits>

bool TerrainSettingsPanel::draw(TerrainSettings& draftSettings, Rectangle bounds) {
    GuiPanel(bounds, "Terrain Settings");

    constexpr float padding = 20.0f;
    constexpr float controlHeight = 36.0f;
    constexpr float rowSpacing = 10.0f;
    constexpr float labelWidth = 120.0f;
    constexpr float sliderValueWidth = 80.0f;
    constexpr float sliderValueGap = 10.0f;
    constexpr int minimumMapSize = 64;
    constexpr int maximumMapSize = 2048;

    const float contentX = bounds.x + padding;
    const float contentWidth = bounds.width - padding * 2.0f;
    const float controlX = contentX + labelWidth;
    const float inputWidth = contentWidth - labelWidth;

    float currentY = bounds.y + 52.0f;

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
        Rectangle{
            controlX,
            currentY,
            inputWidth - sliderValueWidth - sliderValueGap,
            controlHeight
        },
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
        Rectangle{
            controlX,
            currentY,
            inputWidth - sliderValueWidth - sliderValueGap,
            controlHeight
        },
        nullptr,
        TextFormat("%.2f", draftSettings.persistence),
        &draftSettings.persistence,
        0.0f,
        1.0f
    );
    advanceRow();

    drawLabel("Lacunarity");
    GuiSliderBar(
        Rectangle{
            controlX,
            currentY,
            inputWidth - sliderValueWidth - sliderValueGap,
            controlHeight
        },
        nullptr,
        TextFormat("%.2f", draftSettings.lacunarity),
        &draftSettings.lacunarity,
        1.0f,
        4.0f
    );
    advanceRow();

    currentY += 4.0f;

    const bool resetPressed = GuiButton(
        Rectangle{
            contentX,
            currentY,
            contentWidth,
            controlHeight
        },
        "Reset to Defaults"
    );

    if (resetPressed) {
        draftSettings = TerrainSettings{};
        widthEditMode_ = false;
        heightEditMode_ = false;
        seedEditMode_ = false;
        octavesEditMode_ = false;
    }

    advanceRow();

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
