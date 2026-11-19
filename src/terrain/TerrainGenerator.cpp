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

}
