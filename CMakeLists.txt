cmake_minimum_required(VERSION 3.19)
project(NapptendoGB C)

set(CMAKE_C_STANDARD 90)

add_executable(NapptendoGB main.c bus.c bus.h cpu.c cpu.h interrupts.c interrupts.h gpu.c gpu.h timers.c timers.h controls.c controls.h debug.c debug.h)
add_compile_options(-Wimplicit-fallthrough -Wall -Wextra -pedantic -Werror)
target_link_libraries(NapptendoGB -lmingw32 -lSDL2main -lSDL2)