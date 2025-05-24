file(GLOB HEADER_LIST "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB SOURCE_LIST "${CMAKE_SOURCE_DIR}/src/*.c")

add_library(ngb_lib ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(ngb_lib PUBLIC ../include)