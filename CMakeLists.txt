cmake_minimum_required(VERSION 4.2)
project(wbtb LANGUAGES C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Executable
add_executable(wbtb)
target_sources(wbtb PRIVATE src/main.c src/wbl.c)
target_include_directories(wbtb PRIVATE include)