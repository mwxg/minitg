#pragma once

#include <raylib.h>

#include <string>

#include "../rendering/TerrainView.hpp"
#include "../terrain/TerrainSettings.hpp"

class TerrainSettingsPanel {
public:
    bool draw(
        TerrainSettings& settings,
        TerrainView& terrainView,
        Rectangle bounds,
        const std::string& generationError
    );

private:
    bool widthEditMode_ = false;
    bool heightEditMode_ = false;
    bool seedEditMode_ = false;
    bool elevationOctavesEditMode_ = false;
    bool moistureOctavesEditMode_ = false;
    bool viewDropdownOpen_ = false;
    int viewSelection_ = 0;
    Vector2 scrollOffset_{};
};
