cmake_minimum_required(VERSION 3.27)

project(minitg VERSION 1.0.0 LANGUAGES CXX)

# Keep Windows builds self-contained. MSVC otherwise uses the DLL-based C/C++
# runtime, which requires the matching Visual C++ Redistributable on the user's
# computer. This setting is declared before dependencies are created so raylib
# uses the same runtime as MiniTG.
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

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

add_executable(minitg WIN32
    src/main.cpp
    src/gui/Raygui.cpp
    src/gui/TerrainSettingsPanel.cpp
    src/rendering/TerrainRenderer.cpp
    src/terrain/TerrainMap.cpp
    src/terrain/TerrainClassifier.cpp
    src/terrain/TerrainGenerator.cpp
)

if(WIN32)
    file(TO_CMAKE_PATH
        "${CMAKE_SOURCE_DIR}/resources/windows/minitgLogo.ico"
        MINITG_ICON_PATH
    )
    configure_file(
        resources/windows/minitg.rc.in
        "${CMAKE_CURRENT_BINARY_DIR}/minitg.rc"
        @ONLY
    )
    target_sources(minitg PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/minitg.rc"
    )
endif()

target_sources(minitg PRIVATE
    src/rendering/TerrainRenderer.hpp
    src/rendering/TerrainView.hpp
    src/terrain/TerrainMap.hpp
    src/terrain/TerrainSettings.hpp
    src/terrain/TerrainType.hpp
    src/terrain/TerrainClassifier.hpp
    src/terrain/TerrainGenerator.hpp
    src/gui/TerrainSettingsPanel.hpp
)

target_include_directories(minitg PRIVATE
    ${raygui_SOURCE_DIR}/src
)

target_link_libraries(minitg PRIVATE raylib)

if(MINGW)
    target_link_options(minitg PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
    )
endif()

target_compile_features(minitg PRIVATE cxx_std_20)

add_custom_command(TARGET minitg POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:minitg>/assets"
    COMMENT "Copying runtime assets"
)

install(
    TARGETS minitg
    RUNTIME DESTINATION .
    COMPONENT Runtime
)

install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/assets"
    DESTINATION .
    COMPONENT Runtime
)

install(
    FILES
        "${CMAKE_SOURCE_DIR}/LICENSE"
        "${CMAKE_SOURCE_DIR}/THIRD_PARTY_NOTICES.md"
    DESTINATION .
    COMPONENT Runtime
)

install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/licenses"
    DESTINATION .
    COMPONENT Runtime
)

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

set(CPACK_PACKAGE_NAME "MiniTG")
set(CPACK_PACKAGE_VENDOR "MiniTG")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "A top-down biome and terrain generator"
)

set(CPACK_PACKAGE_INSTALL_DIRECTORY "MiniTG")
set(CPACK_PACKAGE_FILE_NAME
    "MiniTG-${PROJECT_VERSION}-win64"
)

# create a MiniTG shortcut in the Windows start menu
set(CPACK_PACKAGE_EXECUTABLES
    "minitg" "MiniTG"
)

# MiniTG is installed directly in the installation root, not bin/
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")

# create a desktop shortcut during installation and remove it during
# uninstallation. Using explicit NSIS commands avoids depending on CPack's
# optional desktop-shortcut page.
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut \\\"$DESKTOP\\\\MiniTG.lnk\\\" \\\"$INSTDIR\\\\minitg.exe\\\""
)
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete \\\"$DESKTOP\\\\MiniTG.lnk\\\""
)

set(CPACK_GENERATOR "ZIP;NSIS")

set(CPACK_NSIS_DISPLAY_NAME "MiniTG")
set(CPACK_NSIS_PACKAGE_NAME "MiniTG")
set(CPACK_NSIS_MUI_ICON
    "${CMAKE_SOURCE_DIR}/resources/windows/minitgLogo.ico"
)
set(CPACK_NSIS_MUI_UNIICON
    "${CMAKE_SOURCE_DIR}/resources/windows/minitgLogo.ico"
)

set(CPACK_COMPONENTS_ALL Runtime)

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY ON)

set(CPACK_RESOURCE_FILE_LICENSE
    "${CMAKE_SOURCE_DIR}/LICENSE"
)

include(CPack)
