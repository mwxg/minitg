#pragma once

#include <raylib.h>

#include "../terrain/TerrainSettings.hpp"

class TerrainSettingsPanel {
public:
    bool draw(TerrainSettings& settings, Rectangle bounds);

private:
    bool mapSizeEditMode_ = false;
    bool seedEditMode_ = false;
    bool octavesEditMode_ = false;
};