project(BuBox2D)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/modules)

# Source files
set(BINDING_SOURCES
    src/bindings.cpp
)

# Box2D from external folder
set(BOX2D_DIR ${CMAKE_SOURCE_DIR}/external/box2d)

# ============================================
# bu_box2d Plugin (shared library)
# ============================================
add_library(bu_box2d SHARED src/plugin.cpp ${BINDING_SOURCES})

target_include_directories(bu_box2d PRIVATE
    src
    ${CMAKE_SOURCE_DIR}/libbu/include
    ${BOX2D_DIR}/include
)

target_compile_definitions(bu_box2d PRIVATE BU_PLUGIN_BUILD)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    if (UNIX)
        target_compile_options(bu_box2d PRIVATE -g -D_DEBUG -DVERBOSE -fPIC)
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(bu_box2d PRIVATE -O3 -DNDEBUG -fPIC)
endif()

if (UNIX)
    set_target_properties(bu_box2d PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

set_target_properties(bu_box2d PROPERTIES
    OUTPUT_NAME "bu_box2d"
    PREFIX "lib"
)

# Link libraries
if (WIN32)
    target_link_libraries(bu_box2d libbu box2d)
else()
    # On Unix, symbols will be resolved from the main executable
    target_link_libraries(bu_box2d box2d m dl)
endif()
