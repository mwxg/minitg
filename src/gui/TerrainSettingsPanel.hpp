#pragma once

#include <raylib.h>

#include "../rendering/TerrainView.hpp"
#include "../terrain/TerrainSettings.hpp"

class TerrainSettingsPanel {
public:
    bool draw(
        TerrainSettings& settings,
        TerrainView& terrainView,
        Rectangle bounds
    );

private:
    bool widthEditMode_ = false;
    bool heightEditMode_ = false;
    bool seedEditMode_ = false;
    bool octavesEditMode_ = false;
    bool viewDropdownOpen_ = false;
    int viewSelection_ = 0;
};
