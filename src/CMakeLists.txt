set(HEADER_LIST 
    "${CMAKE_SOURCE_DIR}/include/NapptendoGB/bus.h"
    "${CMAKE_SOURCE_DIR}/include/NapptendoGB/timers.h"
    "${CMAKE_SOURCE_DIR}/include/NapptendoGB/cpu.h"
    "${CMAKE_SOURCE_DIR}/include/NapptendoGB/debug.h"
    "${CMAKE_SOURCE_DIR}/include/NapptendoGB/gpu.h"
    "${CMAKE_SOURCE_DIR}/include/NapptendoGB/controls.h"
    "${CMAKE_SOURCE_DIR}/include/NapptendoGB/interrupts.h")

set(SOURCE_LIST
    "${CMAKE_SOURCE_DIR}/src/interrupts.c"
    "${CMAKE_SOURCE_DIR}/src/bus.c"
    "${CMAKE_SOURCE_DIR}/src/controls.c"
    "${CMAKE_SOURCE_DIR}/src/cpu.c"
    "${CMAKE_SOURCE_DIR}/src/debug.c"
    "${CMAKE_SOURCE_DIR}/src/gpu.c"
    "${CMAKE_SOURCE_DIR}/src/timers.c")

add_library(ngb_lib ${SOURCE_LIST} ${HEADER_LIST})

target_include_directories(ngb_lib PUBLIC ../include)