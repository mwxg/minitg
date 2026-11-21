cmake_minimum_required(VERSION 3.27)

project(minitg VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)

set(RAYLIB_VERSION 6.2)
set(RAYGUI_VERSION 5.0)

FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    FIND_PACKAGE_ARGS
)
FetchContent_Declare(
    raygui
    URL https://github.com/raysan5/raygui/archive/refs/tags/${RAYGUI_VERSION}.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
)

FetchContent_MakeAvailable(raylib raygui)

add_executable(minitg
    src/main.cpp
    src/gui/Raygui.cpp
    src/rendering/TerrainRenderer.cpp
    src/terrain/TerrainMap.cpp
    src/terrain/TerrainClassifier.cpp
    src/terrain/TerrainGenerator.cpp
)

target_sources(minitg PRIVATE
    src/rendering/TerrainRenderer.hpp
    src/terrain/TerrainMap.hpp
    src/terrain/TerrainSettings.hpp
    src/terrain/TerrainType.hpp
    src/terrain/TerrainClassifier.hpp
    src/terrain/TerrainGenerator.hpp
)

target_include_directories(minitg PRIVATE
    ${raygui_SOURCE_DIR}/src
)

target_link_libraries(minitg PRIVATE raylib)

target_compile_features(minitg PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(minitg PRIVATE /W4 /permissive-)
else()
    target_compile_options(minitg PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(MSVC)
    set_source_files_properties(
        src/gui/Raygui.cpp
        PROPERTIES COMPILE_OPTIONS "/wd4505"
    )
else()
    set_source_files_properties(
        src/gui/Raygui.cpp
        PROPERTIES COMPILE_OPTIONS "-Wno-unused-function"
    )
endif()
