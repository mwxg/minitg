# MiniTG

MiniTG is a simple top-down terrain generator.

It uses deterministic, coordinate-hashed value noise with smooth interpolation
to generate elevation values. Multiple noise octaves are combined using fractal
Brownian motion (fBm), adding progressively finer terrain detail before each
elevation is classified as deep water, water, sand, grass, rock, or snow.

A single world seed controls terrain generation. The elevation field uses the
world seed directly, while the generator derives a separate deterministic seed
for the moisture field so both maps vary together without sharing the same
noise pattern.

An optional island falloff gradually lowers elevation toward the edges of the
map, creating a central landmass surrounded by water. Its radius and strength
can be adjusted from the terrain settings panel.


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
