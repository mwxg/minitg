#include "TerrainGenerator.hpp"

#include "TerrainClassifier.hpp"

#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <algorithm>

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

// combine multiple layers (octaves) of noise into one normalized
// elevation sample
// low values represent lower terrain and vice versa
float sampleFractalNoise(float x, float y, const NoiseSettings& settings) noexcept {
    // track both the weighted noise and the total weight so the final result
    // remains in [0, 1] regardless of how many octaves are used
    float total = 0.0f;
    float totalAmplitude = 0.0f;

    float frequency = settings.frequency;
    float amplitude = 1.0f;
    for (int octave = 0; octave < settings.octaves; octave++) {
        // each octave samples the same deterministic noise field at a
        // progressively finer scale
        const float noise = sampleValueNoise(
            x * frequency,
            y * frequency,
            static_cast<std::uint32_t>(settings.seed)
        );

        // larger amplitudes give an octave more influence over the elevation
        total += noise * amplitude;
        totalAmplitude += amplitude;

        // the next octave has smaller features and normally less influence
        frequency *= settings.lacunarity;
        amplitude *= settings.persistence;
    }

    // convert the weighted sum back into a normalized elevation value
    return total / totalAmplitude;
}

void validateTerrainSettings(const TerrainSettings& settings) {
    if (settings.width < 2 || settings.height < 2) {
        throw std::invalid_argument(
            "Terrain dimensions must be at least 2"
        );
    }

    if (!std::isfinite(settings.frequency) ||
        settings.frequency <= 0.0f) {
        throw std::invalid_argument(
            "Terrain frequency must be positive and finite"
        );
    }

    if (settings.octaves <= 0) {
        throw std::invalid_argument(
            "Terrain must have at least one octave"
        );
    }

    if (!std::isfinite(settings.persistence) ||
        settings.persistence < 0.0f ||
        settings.persistence > 1.0f) {
        throw std::invalid_argument(
            "Terrain persistence must be between 0 and 1"
        );
    }

    if (!std::isfinite(settings.lacunarity) ||
        settings.lacunarity < 1.0f) {
        throw std::invalid_argument(
            "Terrain lacunarity must be at least 1"
        );
    }

    if (!std::isfinite(settings.islandRadius) ||
        settings.islandRadius <= 0.0f ||
        settings.islandRadius >= 1.0f) {
        throw std::invalid_argument(
            "Island radius must be between 0 and 1"
        );
    }

    if (!std::isfinite(settings.islandFalloffStrength) ||
        settings.islandFalloffStrength < 0.0f ||
        settings.islandFalloffStrength > 1.0f) {
        throw std::invalid_argument(
            "Island falloff strength must be between 0 and 1"
        );
    }

    const TerrainThresholds& thresholds = settings.thresholds;

    if (!std::isfinite(thresholds.deepWater) ||
        !std::isfinite(thresholds.water) ||
        !std::isfinite(thresholds.sand) ||
        !std::isfinite(thresholds.grass) ||
        !std::isfinite(thresholds.rock)) {
        throw std::invalid_argument(
            "Terrain thresholds must be finite"
        );
    }

    if (thresholds.deepWater < 0.0f ||
        thresholds.rock > 1.0f) {
        throw std::invalid_argument(
            "Terrain thresholds must be between 0 and 1"
        );
    }

    if (!(thresholds.deepWater < thresholds.water &&
          thresholds.water < thresholds.sand &&
          thresholds.sand < thresholds.grass &&
          thresholds.grass < thresholds.rock)) {
        throw std::invalid_argument(
            "Terrain thresholds must be strictly increasing"
        );
    }
}

float smoothstep(float edge0, float edge1, float value) noexcept {
    const float t = std::clamp((value - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

} // namespace

TerrainMap generateTerrain(const TerrainSettings& settings) {
    validateTerrainSettings(settings);

    TerrainMap map(settings);

    const NoiseSettings elevationNoise{
        static_cast<std::uint32_t>(settings.seed),
        settings.frequency,
        settings.octaves,
        settings.persistence,
        settings.lacunarity
    };

    NoiseSettings moistureNoise = settings.moistureNoise;

    // derive moisture seed from world seed
    // so they have different maps
    moistureNoise.seed = static_cast<std::uint32_t>(settings.seed) ^ 0x9E3779B9u;

    for (int y = 0; y < map.height(); y++) {
        for (int x = 0; x < map.width(); x++) {
            float elevation = sampleFractalNoise(
                static_cast<float>(x),
                static_cast<float>(y),
                elevationNoise
            );

            const float moisture = sampleFractalNoise(
                static_cast<float>(x),
                static_cast<float>(y),
                moistureNoise
            );

            if (settings.islandFalloffEnabled) {
                // map coordinates to a centered range
                const float normalizedX =
                    (2.0f * static_cast<float>(x) /
                     static_cast<float>(map.width() - 1)) - 1.0f;
                const float normalizedY =
                    (2.0f * static_cast<float>(y) /
                     static_cast<float>(map.height() - 1)) - 1.0f;

                const float distanceFromCenter = std::sqrt(
                    normalizedX * normalizedX +
                    normalizedY * normalizedY
                );

                const float falloff = smoothstep(
                    settings.islandRadius,
                    1.0f,
                    distanceFromCenter
                );

                elevation = std::clamp(
                    elevation - falloff * settings.islandFalloffStrength,
                    0.0f,
                    1.0f
                );
            }

            TerrainCell& cell = map.at(x, y);
            cell.elevation = elevation;
            cell.moisture = moisture;
            cell.type = classifyTerrain(elevation, settings.thresholds);
        }
    }

    return map;
}
