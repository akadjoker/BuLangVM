project(BuOgre3D)

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

# Find Ogre3D
# find_package(OGRE REQUIRED)

# ============================================
# bu_ogre3d Plugin (shared library)
# ============================================
add_library(bu_ogre3d SHARED src/plugin.cpp ${BINDING_SOURCES})

target_include_directories(bu_ogre3d PRIVATE
    src
    ${CMAKE_SOURCE_DIR}/libbu/include
    # ${OGRE_INCLUDE_DIRS}
)

target_compile_definitions(bu_ogre3d PRIVATE BU_PLUGIN_BUILD)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    if (UNIX)
        target_compile_options(bu_ogre3d PRIVATE -g -D_DEBUG -DVERBOSE -fPIC)
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(bu_ogre3d PRIVATE -O3 -DNDEBUG -fPIC)
endif()

if (UNIX)
    set_target_properties(bu_ogre3d PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

set_target_properties(bu_ogre3d PROPERTIES
    OUTPUT_NAME "bu_ogre3d"
    PREFIX "lib"
)

# Link libraries
if (WIN32)
    target_link_libraries(bu_ogre3d libbu)
    # target_link_libraries(bu_ogre3d libbu ${OGRE_LIBRARIES})
else()
    # On Unix, symbols will be resolved from the main executable
    target_link_libraries(bu_ogre3d m dl)
    # target_link_libraries(bu_ogre3d ${OGRE_LIBRARIES} m dl)
endif()
