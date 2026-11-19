#include "TerrainGenerator.hpp"

#include "TerrainClassifier.hpp"

#include <cmath>
#include <cstdint>
#include <limits>

namespace {

std::uint32_t hashCoordinates(
    int x,
    int y,
    std::uint32_t seed
) noexcept {
    // Begin with the seed so the same coordinate can produce a different,
    // but still deterministic, value for each generated world.
    std::uint32_t hash = seed;

    // Convert each coordinate to an unsigned 32-bit value, multiply it by a
    // different odd constant, and XOR the result into the hash. Unsigned
    // arithmetic deliberately wraps modulo 2^32, so negative coordinates and
    // multiplication overflow are both well-defined. Using a distinct constant
    // for each axis prevents (x, y) and (y, x) from being mixed identically.
    hash ^= static_cast<std::uint32_t>(x) * 0x27d4eb2du;
    hash ^= static_cast<std::uint32_t>(y) * 0x165667b1u;

    // Avalanche the combined bits. The shifts fold high bits into low bits,
    // while the multiplications spread each changed input bit across the word.
    // Consequently, nearby coordinates usually produce very different values
    // instead of an obvious spatial pattern. All constants have a 'u' suffix
    // to keep every operation in the intended unsigned 32-bit domain.
    hash ^= hash >> 16;
    hash *= 0x7feb352du;
    hash ^= hash >> 15;
    hash *= 0x846ca68bu;
    hash ^= hash >> 16;

    // This is a coordinate hash, not a stateful random-number generator: the
    // same (x, y, seed) always returns the same value, independent of call order.
    return hash;
}

// normalize hash to [0, 1]
float hashToUnitFloat(std::uint32_t hash) noexcept {
    constexpr double maxHash = static_cast<double>(std::numeric_limits<std::uint32_t>::max());

    // perform division as double so the normalization is more accurate
    return static_cast<float>(static_cast<double>(hash) / maxHash);
}

float latticeValue(int x, int y, std::uint32_t seed) noexcept {
    return hashToUnitFloat(hashCoordinates(x, y, seed));
}

// nonlinearly remap position so the transition between 
// lattice points is an eased one
// leave starting point slow, speed up, slow down before reaching second point
float fade(float t) noexcept {
    return t * t * (3.0f - 2.0f * t);
}

// ex. lerp(0.2f, 0.8f, 0.5f) == 0.5 (linear interpolation)
float lerp(float a, float b, float t) noexcept {
    return a + (b - a) * t;
}

float sampleValueNoise(float x, float y, std::uint32_t seed) noexcept {
    
    // find the surrounding lattice square
    const int x0 = static_cast<int>(std::floor(x));
    const int y0 = static_cast<int>(std::floor(y));
    const int x1 = x0 + 1;
    const int y1 = y0 + 1;

    // find local position inside the lattice square
    const float localX = x - static_cast<float>(x0);
    const float localY = y - static_cast<float>(y0);

    const float blendX = fade(localX);
    const float blendY = fade(localY);

    const float value00 = latticeValue(x0, y0, seed);
    const float value10 = latticeValue(x1, y0, seed);
    const float value01 = latticeValue(x0, y1, seed);
    const float value11 = latticeValue(x1, y1, seed);

    // top and bottom row of box
    /*
    value00 ─────── row0 ───────── value10

    value01 ─────── row1 ───────── value11
    */
    const float row0 = lerp(value00, value10, blendX);
    const float row1 = lerp(value01, value11, blendX);

    // blend the two row results 
    return lerp(row0, row1, blendY);
}

}
