cmake_minimum_required(VERSION 3.10)
project(bulang_visual_test)

set(CMAKE_CXX_STANDARD 14)

# Project root is one level up
set(BULANG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")

# Find raylib
find_library(RAYLIB_LIB raylib PATHS /usr/local/lib)
find_path(RAYLIB_INCLUDE raylib.h PATHS /usr/local/include)

if(NOT RAYLIB_LIB OR NOT RAYLIB_INCLUDE)
    message(FATAL_ERROR "raylib not found! Install it or set paths.")
endif()

add_executable(bulang_visual main.cpp)

target_include_directories(bulang_visual PRIVATE
    ${BULANG_ROOT}/libbu/include
    ${RAYLIB_INCLUDE}
)

target_link_libraries(bulang_visual PRIVATE
    ${BULANG_ROOT}/bin/libbu.a
    ${BULANG_ROOT}/build/vendor/miniz/libminiz.a
    ${RAYLIB_LIB}
    m dl pthread
)
