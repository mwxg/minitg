# MiniTG

MiniTG is a simple top-down biome and terrain generator.

It uses deterministic, coordinate-hashed value noise with smooth interpolation
to generate elevation values. Multiple noise octaves are combined using fractal
Brownian motion (fBm), adding progressively finer terrain detail before each
elevation is combined with moisture and temperature to classify each cell as
one of 13 terrain types.

A single world seed controls terrain generation. The elevation field uses the
world seed directly, while the generator derives separate deterministic seeds
for the moisture and temperature fields. This keeps all three maps tied to the
same world seed without giving them identical noise patterns. The application
can display the resulting terrain, moisture bands, temperature field, or
grayscale elevation bands.

An optional island falloff gradually lowers elevation toward the edges of the
map, creating a central landmass surrounded by water. Its radius and strength
can be adjusted from the terrain settings panel.

## Noise settings

Elevation, moisture, and temperature each have their own fBm noise settings.
The generator starts with one layer of value noise, then adds more layers
(`octaves`) at progressively different scales and strengths. It divides the
combined result by the total strength of the layers, keeping the result in the
`0.00` to `1.00` range.

- **Frequency** controls the scale of the first noise layer. A lower frequency
  produces larger, smoother regions; a higher frequency produces smaller,
  more rapidly changing regions. For example, a frequency of `0.015` makes
  the first layer span roughly `1 / 0.015`, or 67 cells, per noise interval.

- **Octaves** is the number of noise layers combined. The first octave creates
  the broad shape, while each additional octave can add finer detail. More
  octaves generally make the map more detailed and also increase generation
  cost.

- **Persistence** multiplies an octave's strength to get the strength of the
  next octave. With the default value of `0.5`, successive strengths are `1`,
  `0.5`, `0.25`, and so on. Lower values make the broad first octave dominate;
  higher values give fine details more influence. It must be between `0` and
  `1`.

- **Lacunarity** multiplies an octave's frequency to get the frequency of the
  next octave. With the default value of `2.0`, each layer has features about
  half the size of the preceding layer. A value closer to `1` keeps successive
  layers at similar scales; a larger value creates a wider jump between broad
  shapes and fine details. It must be at least `1`.

For an octave numbered `n` starting at zero, its frequency is
`frequency * lacunarity^n` and its strength is `persistence^n`.

## Biome colors

The terrain view uses the following colors:

![MiniTG biome color palette](screenshots/biome-colors.svg)

## Terrain classification

Elevation, moisture, and temperature are normalized to the range `0.00` to
`1.00`. The table below describes classification with the default settings;
moisture and temperature thresholds can be changed in the terrain settings panel. `Any` means that
the value does not affect that terrain type.

You may notice that several terrain types share the same elevation ranges. This is because there are six "default" ones: Deep water, water, sand, grass, rock, and snow. The rest of the terrain types are essentially applied on top of them.
e.g. grass + dry + cold = tundra.

| Terrain type | Elevation | Moisture | Temperature |
| --- | --- | --- | --- |
| Deep water | `< 0.25` | Any | `>= 0.35` |
| Water | `>= 0.25` and `< 0.40` | Any | `>= 0.35` |
| Frozen water | `< 0.40` | Any | `< 0.35` |
| Sand | `>= 0.40` and `< 0.45` | Any | Any |
| Desert | `>= 0.45` and `< 0.70` | `< 0.45` (dry) | `>= 0.50` (hot) |
| Savanna | `>= 0.45` and `< 0.70` | `>= 0.45` and `< 0.65` (moderate) | `>= 0.50` (hot) |
| Grass | `>= 0.45` and `< 0.70` | `< 0.65` (dry or moderate) | `>= 0.35` and `< 0.50` (temperate) |
| Forest | `>= 0.45` and `< 0.70` | `>= 0.65` (wet) | `>= 0.35` and `< 0.50` (temperate) |
| Rainforest | `>= 0.45` and `< 0.70` | `>= 0.65` (wet) | `>= 0.50` (hot) |
| Taiga | `>= 0.45` and `< 0.70` | `>= 0.45` (moderate or wet) | `< 0.35` (cold) |
| Tundra | `>= 0.45` and `< 0.70` | `< 0.45` (dry) | `< 0.35` (cold) |
| Rock | `>= 0.70` and `< 0.85` | Any | `>= 0.35` |
| Snow | `>= 0.85`, or `>= 0.70` and `< 0.85` when cold | Any | Any at `>= 0.85`; otherwise `< 0.35` |

The default named bands are dry (`< 0.45`), moderate (`>= 0.45` and
`< 0.65`), and wet (`>= 0.65`) for moisture; and cold (`< 0.35`),
temperate (`>= 0.35` and `< 0.50`), and hot (`>= 0.50`) for temperature.


## Building

Requirements:

- CMake 3.27+
- Ninja
- A C++20 compiler

```sh
cmake --preset debug
cmake --build --preset debug
.\build\debug\minitg.exe
```
