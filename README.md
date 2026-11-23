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
can display the resulting terrain, moisture bands, or temperature field.

An optional island falloff gradually lowers elevation toward the edges of the
map, creating a central landmass surrounded by water. Its radius and strength
can be adjusted from the terrain settings panel.

## Biome colors

The terrain view uses the following colors:

| Color | Terrain type | RGB | Hex |
| --- | --- | --- | --- |
| <span style="display:inline-block;width:1.25em;height:1.25em;background:#143778;border:1px solid #777"></span> | Deep water | `20, 55, 120` | `#143778` |
| <span style="display:inline-block;width:1.25em;height:1.25em;background:#236EB4;border:1px solid #777"></span> | Water | `35, 110, 180` | `#236EB4` |
| <span style="display:inline-block;width:1.25em;height:1.25em;background:#A5CDE1;border:1px solid #777"></span> | Frozen water | `165, 205, 225` | `#A5CDE1` |
| <span style="display:inline-block;width:1.25em;height:1.25em;background:#DCCD8C;border:1px solid #777"></span> | Sand | `220, 205, 140` | `#DCCD8C` |
| <span style="display:inline-block;width:1.25em;height:1.25em;background:#CDB469;border:1px solid #777"></span> | Desert | `205, 180, 105` | `#CDB469` |
| <span style="display:inline-block;width:1.25em;height:1.25em;background:#A5AF4B;border:1px solid #777"></span> | Savanna | `165, 175, 75` | `#A5AF4B` |
| <span style="display:inline-block;width:1.25em;height:1.25em;background:#46914B;border:1px solid #777"></span> | Grass | `70, 145, 75` | `#46914B` |
| <span style="display:inline-block;width:1.25em;height:1.25em;background:#196946;border:1px solid #777"></span> | Forest | `25, 105, 70` | `#196946` |
| <span style="display:inline-block;width:1.25em;height:1.25em;background:#0A4B23;border:1px solid #777"></span> | Rainforest | `10, 75, 35` | `#0A4B23` |
| <span style="display:inline-block;width:1.25em;height:1.25em;background:#376464;border:1px solid #777"></span> | Taiga | `55, 100, 100` | `#376464` |
| <span style="display:inline-block;width:1.25em;height:1.25em;background:#96A087;border:1px solid #777"></span> | Tundra | `150, 160, 135` | `#96A087` |
| <span style="display:inline-block;width:1.25em;height:1.25em;background:#736E69;border:1px solid #777"></span> | Rock | `115, 110, 105` | `#736E69` |
| <span style="display:inline-block;width:1.25em;height:1.25em;background:#F0F5FA;border:1px solid #777"></span> | Snow | `240, 245, 250` | `#F0F5FA` |

## Terrain classification

Elevation, moisture, and temperature are normalized to the range `0.00` to
`1.00`. The table below describes classification with the default settings;
the thresholds can be changed in the terrain settings panel. `Any` means that
the value does not affect that terrain type.

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
