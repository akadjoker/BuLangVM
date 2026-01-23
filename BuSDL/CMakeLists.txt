project(BuSDL)
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

 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/modules)

 
set(BINDING_SOURCES
    src/bindings.cpp
    src/core.cpp
    src/renderer.cpp
    src/window.cpp
    src/opengl.cpp
)

find_package(OpenGL REQUIRED)

 

# ============================================
# libbu_sdl Plugin (shared library)
# ============================================
add_library(bu_sdl SHARED src/plugin.cpp ${BINDING_SOURCES})

target_include_directories(bu_sdl PRIVATE include src ${CMAKE_SOURCE_DIR}/libbu/include)

# Plugin-specific compile definitions
target_compile_definitions(bu_sdl PRIVATE BU_PLUGIN_BUILD)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    if (UNIX)
        target_compile_options(bu_sdl PRIVATE -g -D_DEBUG -DVERBOSE -fPIC)
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(bu_sdl PRIVATE -O3 -DNDEBUG -fPIC)
endif()

# On Unix, shared libraries need -fPIC
if (UNIX)
    set_target_properties(bu_sdl PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Set output name to libbu_sdl.so (or .dll on Windows)
set_target_properties(bu_sdl PROPERTIES
    OUTPUT_NAME "bu_sdl"
    PREFIX "lib"
)

# Don't link libbu to the plugin - it will use symbols from the main executable
# This avoids duplicate state issues with static libraries
if (WIN32)
    # On Windows, we need to link libbu since we can't use -rdynamic
    target_link_libraries(bu_sdl libbu SDL2 OpenGL::GL Winmm.lib)
else()
    # On Unix, symbols will be resolved from the main executable
    target_link_libraries(bu_sdl SDL2 OpenGL::GL m dl)
endif()
