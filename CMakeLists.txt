cmake_minimum_required(VERSION 3.27)

project(minitg VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)

set(RAYLIB_VERSION 6.2)

FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(raylib)

add_executable(minitg
    src/main.cpp
)

target_link_libraries(minitg PRIVATE raylib)

target_compile_features(minitg PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(minitg PRIVATE /W4 /permissive-)
else()
    target_compile_options(minitg PRIVATE -Wall -Wextra -Wpedantic)
endif()
