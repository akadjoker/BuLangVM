project(BuGLFW)
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
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
 
set(IMGUI_SOURCES
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

set(BINDING_SOURCES
    src/bindings.cpp
    src/core.cpp
    src/window.cpp
    src/input.cpp
    src/monitor.cpp
    src/context.cpp
 
)
 
# ============================================
#  Plugin (shared library)
# ============================================
add_library(bu_glfw SHARED src/plugin.cpp ${BINDING_SOURCES}  ${IMGUI_SOURCES})

target_include_directories(bu_glfw PRIVATE include src ${CMAKE_SOURCE_DIR}/libbu/include ${IMGUI_DIR} ${IMGUI_DIR}/backends ${IMGUI_DIR}/include)

# Plugin-specific compile definitions
target_compile_definitions(bu_glfw PRIVATE BU_PLUGIN_BUILD)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    if (UNIX)
        target_compile_options(bu_glfw PRIVATE -g -D_DEBUG -DVERBOSE -fPIC)
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(bu_glfw PRIVATE -O3 -DNDEBUG -fPIC)
endif()

# On Unix, shared libraries need -fPIC
if (UNIX)
    set_target_properties(bu_glfw PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Set output name to libbu_raylib.so (or .dll on Windows)
set_target_properties(bu_glfw PROPERTIES
    OUTPUT_NAME "bu_glfw"
    PREFIX "lib"
)

# Don't link libbu to the plugin - it will use symbols from the main executable
# This avoids duplicate state issues with static libraries
if (WIN32)
    # On Windows, we need to link libbu since we can't use -rdynamic
    target_link_libraries(bu_glfw libbu glfw imgui Winmm.lib)
else()
    # On Unix, symbols will be resolved from the main executable
    target_link_libraries(bu_glfw glfw imgui m dl)
endif()
