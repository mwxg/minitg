#include "TerrainMap.hpp"

#include <cstddef>
#include <stdexcept>

TerrainMap::TerrainMap(const TerrainSettings& settings) : width_(settings.width), height_(settings.height) {
    if (width_ <= 0 || height_ <= 0) {
        throw std::invalid_argument("Terrain map dimensions must be positive");
    }

    const auto cellCount =
        static_cast<std::size_t>(width_) *
        static_cast<std::size_t>(height_);
    
    cells_.resize(cellCount);
}

int TerrainMap::width() const noexcept {
    return width_;
}
int TerrainMap::height() const noexcept {
    return height_;
}

TerrainCell& TerrainMap::at(int x, int y) {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        throw std::out_of_range("Terrain map coordinates are out of range");
    }

    const auto index =
        static_cast<std::size_t>(y) *
        static_cast<std::size_t>(width_) +
        static_cast<std::size_t>(x);
    
        return cells_[index];
}

const TerrainCell& TerrainMap::at(int x, int y) const
{
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        throw std::out_of_range("Terrain map coordinates are out of range");
    }

    const auto index =
        static_cast<std::size_t>(y) *
        static_cast<std::size_t>(width_) +
        static_cast<std::size_t>(x);

    return cells_[index];
}