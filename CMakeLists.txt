cmake_minimum_required(VERSION 3.19)
project(NGB C)

set(CMAKE_C_STANDARD 99)

# Find SDL2
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

add_subdirectory(src)
add_subdirectory(main)