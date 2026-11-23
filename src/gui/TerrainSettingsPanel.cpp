#include "TerrainSettingsPanel.hpp"

#include <raygui.h>

#include <algorithm>
#include <limits>

bool TerrainSettingsPanel::draw(
    TerrainSettings& draftSettings,
    TerrainView& terrainView,
    Rectangle bounds,
    bool isGenerating,
    const std::string& generationError
) {
    GuiPanel(bounds, "Terrain Settings");

    constexpr float padding = 20.0f;
    constexpr float controlHeight = 36.0f;
    constexpr float rowSpacing = 10.0f;
    constexpr float labelWidth = 200.0f;
    constexpr float sliderValueWidth = 80.0f;
    constexpr float sliderValueGap = 10.0f;
    constexpr float sectionSpacing = 18.0f;
    constexpr float sectionHeaderHeight = 28.0f;
    constexpr float contentHeight = 1550.0f;
    constexpr float errorHeight = 72.0f;
    constexpr int minimumMapSize = 64;
    constexpr int maximumMapSize = 2048;
    constexpr float moistureThresholdGap = 0.01f;
    constexpr float temperatureThresholdGap = 0.01f;

    const Rectangle scrollBounds{
        bounds.x + padding,
        bounds.y + 52.0f,
        bounds.width - padding * 2.0f,
        std::max(0.0f, bounds.height - 52.0f - padding)
    };
    const Rectangle scrollContent{
        0.0f,
        0.0f,
        scrollBounds.width - 16.0f,
        contentHeight + (generationError.empty() ? 0.0f : errorHeight + rowSpacing)
    };
    Rectangle scrollView{};

    GuiScrollPanel(
        scrollBounds,
        nullptr,
        scrollContent,
        &scrollOffset_,
        &scrollView
    );

    // scrollable panel content
    BeginScissorMode(
        static_cast<int>(scrollView.x),
        static_cast<int>(scrollView.y),
        static_cast<int>(scrollView.width),
        static_cast<int>(scrollView.height)
    );

    const float contentX = scrollBounds.x + padding + scrollOffset_.x;
    const float contentWidth = scrollContent.width - padding * 2.0f;
    const float controlX = contentX + labelWidth;
    const float inputWidth = contentWidth - labelWidth;
    float currentY = scrollBounds.y + padding + scrollOffset_.y;

    const auto drawSection = [&](const char* text) {
        GuiLabel(
            Rectangle{contentX, currentY, contentWidth, sectionHeaderHeight},
            text
        );
        currentY += sectionHeaderHeight + rowSpacing;
    };

    const auto drawLabel = [&](const char* text) {
        GuiLabel(
            Rectangle{contentX, currentY, labelWidth, controlHeight},
            text
        );
    };

    const auto advanceRow = [&]() {
        currentY += controlHeight + rowSpacing;
    };

    const auto drawSlider = [&](
        const char* label,
        const char* valueText,
        float* value,
        float minimum,
        float maximum
    ) {
        drawLabel(label);
        GuiSliderBar(
            Rectangle{
                controlX,
                currentY,
                inputWidth - sliderValueWidth - sliderValueGap,
                controlHeight
            },
            nullptr,
            valueText,
            value,
            minimum,
            maximum
        );
        advanceRow();
    };

    if (isGenerating) {
        GuiDisable();
    }

    const bool applyPressed = GuiButton(
        Rectangle{contentX, currentY, contentWidth, controlHeight},
        isGenerating ? "Generating..." : "Generate Terrain"
    );

    if (isGenerating) {
        GuiEnable();
    }

    advanceRow();

    const bool resetPressed = GuiButton(
        Rectangle{contentX, currentY, contentWidth, controlHeight},
        "Reset to Defaults"
    );

    if (resetPressed) {
        draftSettings = TerrainSettings{};
        widthEditMode_ = false;
        heightEditMode_ = false;
        seedEditMode_ = false;
        elevationOctavesEditMode_ = false;
        moistureOctavesEditMode_ = false;
        temperatureOctavesEditMode_ = false;
    }

    advanceRow();

    if (!generationError.empty()) {
        const int previousTextColor = GuiGetStyle(LABEL, TEXT_COLOR_NORMAL);
        const int previousTextAlignmentVertical = GuiGetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL);
        const int previousTextWrapMode = GuiGetStyle(DEFAULT, TEXT_WRAP_MODE);

        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0xC43B3BFF);
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_TOP);
        GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_WORD);
        GuiLabel(
            Rectangle{contentX, currentY, contentWidth, errorHeight},
            generationError.c_str()
        );
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, previousTextColor);
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, previousTextAlignmentVertical);
        GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, previousTextWrapMode);

        currentY += errorHeight + rowSpacing;
    }

    drawLabel("View");
    const Rectangle viewDropdownBounds{
        controlX,
        currentY,
        inputWidth,
        controlHeight
    };
    advanceRow();

    drawSection("GENERAL");

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
        static_cast<std::uint32_t>(std::numeric_limits<int>::max())
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
    draftSettings.seed = static_cast<std::uint32_t>(seedValue);
    advanceRow();

    currentY += sectionSpacing;
    drawSection("ELEVATION");

    drawSlider(
        "Frequency",
        TextFormat("%.3f", draftSettings.elevationNoise.frequency),
        &draftSettings.elevationNoise.frequency,
        0.001f,
        0.1f
    );

    drawLabel("Octaves");
    if (GuiSpinner(
            Rectangle{controlX, currentY, inputWidth, controlHeight},
            nullptr,
            &draftSettings.elevationNoise.octaves,
            1,
            10,
            elevationOctavesEditMode_)) {
        elevationOctavesEditMode_ = !elevationOctavesEditMode_;
    }
    advanceRow();

    drawSlider(
        "Persistence",
        TextFormat("%.2f", draftSettings.elevationNoise.persistence),
        &draftSettings.elevationNoise.persistence,
        0.0f,
        1.0f
    );
    drawSlider(
        "Lacunarity",
        TextFormat("%.2f", draftSettings.elevationNoise.lacunarity),
        &draftSettings.elevationNoise.lacunarity,
        1.0f,
        4.0f
    );

    currentY += sectionSpacing;
    drawSection("MOISTURE");

    drawSlider(
        "Frequency",
        TextFormat("%.3f", draftSettings.moistureNoise.frequency),
        &draftSettings.moistureNoise.frequency,
        0.001f,
        0.1f
    );

    drawLabel("Octaves");
    if (GuiSpinner(
            Rectangle{controlX, currentY, inputWidth, controlHeight},
            nullptr,
            &draftSettings.moistureNoise.octaves,
            1,
            10,
            moistureOctavesEditMode_)) {
        moistureOctavesEditMode_ = !moistureOctavesEditMode_;
    }
    advanceRow();

    drawSlider(
        "Persistence",
        TextFormat("%.2f", draftSettings.moistureNoise.persistence),
        &draftSettings.moistureNoise.persistence,
        0.0f,
        1.0f
    );
    drawSlider(
        "Lacunarity",
        TextFormat("%.2f", draftSettings.moistureNoise.lacunarity),
        &draftSettings.moistureNoise.lacunarity,
        1.0f,
        4.0f
    );

    // keep moisture thresholds strictly ordered
    draftSettings.moistureThresholds.dry = std::clamp(
        draftSettings.moistureThresholds.dry,
        0.0f,
        1.0f - moistureThresholdGap
    );
    draftSettings.moistureThresholds.wet = std::clamp(
        draftSettings.moistureThresholds.wet,
        draftSettings.moistureThresholds.dry + moistureThresholdGap,
        1.0f
    );

    drawSlider(
        "Dry Threshold",
        TextFormat("%.2f", draftSettings.moistureThresholds.dry),
        &draftSettings.moistureThresholds.dry,
        0.0f,
        draftSettings.moistureThresholds.wet - moistureThresholdGap
    );
    drawSlider(
        "Wet Threshold",
        TextFormat("%.2f", draftSettings.moistureThresholds.wet),
        &draftSettings.moistureThresholds.wet,
        draftSettings.moistureThresholds.dry + moistureThresholdGap,
        1.0f
    );

    currentY += sectionSpacing;
    drawSection("TEMPERATURE");

    drawSlider(
        "Frequency",
        TextFormat("%.3f", draftSettings.temperatureNoise.frequency),
        &draftSettings.temperatureNoise.frequency,
        0.001f,
        0.1f
    );

    drawLabel("Octaves");
    if (GuiSpinner(
            Rectangle{controlX, currentY, inputWidth, controlHeight},
            nullptr,
            &draftSettings.temperatureNoise.octaves,
            1,
            10,
            temperatureOctavesEditMode_)) {
        temperatureOctavesEditMode_ = !temperatureOctavesEditMode_;
    }
    advanceRow();

    drawSlider(
        "Persistence",
        TextFormat("%.2f", draftSettings.temperatureNoise.persistence),
        &draftSettings.temperatureNoise.persistence,
        0.0f,
        1.0f
    );
    drawSlider(
        "Lacunarity",
        TextFormat("%.2f", draftSettings.temperatureNoise.lacunarity),
        &draftSettings.temperatureNoise.lacunarity,
        1.0f,
        4.0f
    );

    // keep temperature thresholds strictly ordered
    draftSettings.temperatureThresholds.cold = std::clamp(
        draftSettings.temperatureThresholds.cold,
        0.0f,
        1.0f - temperatureThresholdGap
    );
    draftSettings.temperatureThresholds.hot = std::clamp(
        draftSettings.temperatureThresholds.hot,
        draftSettings.temperatureThresholds.cold + temperatureThresholdGap,
        1.0f
    );

    drawSlider(
        "Cold Threshold",
        TextFormat("%.2f", draftSettings.temperatureThresholds.cold),
        &draftSettings.temperatureThresholds.cold,
        0.0f,
        draftSettings.temperatureThresholds.hot - temperatureThresholdGap
    );
    drawSlider(
        "Hot Threshold",
        TextFormat("%.2f", draftSettings.temperatureThresholds.hot),
        &draftSettings.temperatureThresholds.hot,
        draftSettings.temperatureThresholds.cold + temperatureThresholdGap,
        1.0f
    );
    drawSlider(
        "Elevation Cooling",
        TextFormat("%.2f", draftSettings.elevationCooling),
        &draftSettings.elevationCooling,
        0.0f,
        1.0f
    );

    currentY += sectionSpacing;
    drawSection("ISLAND");

    drawLabel("Falloff");
    GuiCheckBox(
        Rectangle{controlX, currentY, controlHeight, controlHeight},
        nullptr,
        &draftSettings.islandFalloffEnabled
    );
    advanceRow();

    if (!draftSettings.islandFalloffEnabled) {
        GuiDisable();
    }

    drawSlider(
        "Radius",
        TextFormat("%.2f", draftSettings.islandRadius),
        &draftSettings.islandRadius,
        0.01f,
        0.99f
    );
    drawSlider(
        "Falloff Strength",
        TextFormat("%.2f", draftSettings.islandFalloffStrength),
        &draftSettings.islandFalloffStrength,
        0.0f,
        1.0f
    );

    if (!draftSettings.islandFalloffEnabled) {
        GuiEnable();
    }

    if (GuiDropdownBox(
            viewDropdownBounds,
            "Terrain;Moisture;Temperature",
            &viewSelection_,
            viewDropdownOpen_)) {
        viewDropdownOpen_ = !viewDropdownOpen_;
    }

    EndScissorMode();

    terrainView = TerrainView::Terrain;
    if (viewSelection_ == 1) {
        terrainView = TerrainView::Moisture;
    } else if (viewSelection_ == 2) {
        terrainView = TerrainView::Temperature;
    }

    return applyPressed;
}
