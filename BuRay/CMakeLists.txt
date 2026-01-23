project(BuRay)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   -D_CRT_SECURE_NO_WARNINGS")
    if (MSVC)
        if(CMAKE_BUILD_TYPE MATCHES Debug)
            add_compile_options(/RTC1 /Od /Zi)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
        endif()
    endif()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/modules)

# Source files for bindings (shared between executable and plugin)
set(BINDING_SOURCES
    src/bidings.cpp
    src/structs.cpp
    src/core.cpp
    src/shapes.cpp
    src/textures.cpp
    src/text.cpp
    src/input.cpp
    src/audio.cpp
    src/camera.cpp
    src/models.cpp
)

# ============================================
# BuRay Executable (standalone)
# ============================================
add_executable(BuRay src/main.cpp ${BINDING_SOURCES})

target_include_directories(BuRay PRIVATE include src ${CMAKE_SOURCE_DIR}/libbu/include)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    if (UNIX)
        target_compile_options(BuRay PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g -D_DEBUG -DVERBOSE)
        target_link_options(BuRay PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g -D_DEBUG)
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(BuRay PRIVATE -O3 -DNDEBUG -s)
    target_link_options(BuRay PRIVATE -O3 -DNDEBUG)
endif()

target_link_libraries(BuRay libbu raylib)

if (WIN32)
    target_link_libraries(BuRay Winmm.lib)
endif()

if (UNIX)
    target_link_libraries(BuRay m dl)
endif()

# ============================================
# libbu_raylib Plugin (shared library)
# ============================================
add_library(bu_raylib SHARED src/plugin.cpp ${BINDING_SOURCES})

target_include_directories(bu_raylib PRIVATE include src ${CMAKE_SOURCE_DIR}/libbu/include)

# Plugin-specific compile definitions
target_compile_definitions(bu_raylib PRIVATE BU_PLUGIN_BUILD)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    if (UNIX)
        target_compile_options(bu_raylib PRIVATE -g -D_DEBUG -DVERBOSE -fPIC)
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(bu_raylib PRIVATE -O3 -DNDEBUG -fPIC)
endif()

# On Unix, shared libraries need -fPIC
if (UNIX)
    set_target_properties(bu_raylib PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Set output name to libbu_raylib.so (or .dll on Windows)
set_target_properties(bu_raylib PROPERTIES
    OUTPUT_NAME "bu_raylib"
    PREFIX "lib"
)

# Don't link libbu to the plugin - it will use symbols from the main executable
# This avoids duplicate state issues with static libraries
if (WIN32)
    # On Windows, we need to link libbu since we can't use -rdynamic
    target_link_libraries(bu_raylib libbu raylib Winmm.lib)
else()
    # On Unix, symbols will be resolved from the main executable
    target_link_libraries(bu_raylib raylib m dl)
endif()
