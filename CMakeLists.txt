cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0072 NEW)

project(wdiv)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(CLANG_TIDY_EXE NAMES clang-tidy)

if(CLANG_TIDY_EXE)
    add_custom_target(
        tidy
        COMMAND ${CLANG_TIDY_EXE}
        -p=${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/libwdiv/src/*.cpp
        ${CMAKE_SOURCE_DIR}/main/src/*.cpp
        COMMENT "Running clang-tidy"
    )
endif()

add_subdirectory(libbu)
add_subdirectory(external/raylib)

# Enable PIC for raylib and glfw so they can be linked into shared libraries (plugins)
set_target_properties(raylib PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(glfw PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_subdirectory(BuGLFW)
add_subdirectory(BuRay)
add_subdirectory(BuSDL)
add_subdirectory(BuGTK)
add_subdirectory(BuRlGl)
add_subdirectory(bu)


 
