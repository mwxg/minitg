cmake_minimum_required(VERSION 3.27)

project(minitg VERSION 0.1.0 LANGUAGES CXX)

add_executable(minitg
    src/main.cpp
)

target_compile_features(minitg PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(minitg PRIVATE /W4 /permissive-)
else()
    target_compile_options(minitg PRIVATE -Wall -Wextra -Wpedantic)
endif()