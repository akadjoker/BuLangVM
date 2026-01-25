cmake_minimum_required(VERSION 3.10)
project(BuGTK)

cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")

# 1. Encontrar o PkgConfig e depois o GTK3
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS")
    if (MSVC)
        if(CMAKE_BUILD_TYPE MATCHES Debug)
            add_compile_options(/RTC1 /Od /Zi)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
        endif()
    endif()
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/modules)

# Ficheiros de código
set(BINDING_SOURCES
    src/bindings.cpp
    src/plugin.cpp
    
)

add_library(bu_gtk SHARED  src/plugin.cpp ${BINDING_SOURCES})

 
target_include_directories(bu_gtk PRIVATE 
    include 
    src 
    ${CMAKE_SOURCE_DIR}/libbu/include
    ${GTK3_INCLUDE_DIRS} 
)

# Definições específicas do Plugin
target_compile_definitions(bu_gtk PRIVATE BU_PLUGIN_BUILD)

# Flags de compilação
if(CMAKE_BUILD_TYPE MATCHES Debug)
    if (UNIX)
        target_compile_options(bu_gtk PRIVATE -g -D_DEBUG -DVERBOSE -fPIC)
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(bu_gtk PRIVATE -O3 -DNDEBUG -fPIC)
endif()

# Adicionar flags específicas do GTK (Cflags) se necessário
target_compile_options(bu_gtk PRIVATE ${GTK3_CFLAGS_OTHER})

if (UNIX)
    set_target_properties(bu_gtk PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Nome de saída (bu_gtk.dll ou libbnu_gtk.so)
set_target_properties(bu_gtk PROPERTIES
    OUTPUT_NAME "bu_gtk"
    PREFIX "lib"
)

# 3. Linkagem das Bibliotecas
if (WIN32)
    # No Windows, linkamos o GTK e a libbu (core da engine)
    target_link_libraries(bu_gtk 
        ${GTK3_LIBRARIES}
        libbu 
    )
else()
    # No Unix, linkamos apenas o GTK (símbolos da libbu vêm do executável principal)
    target_link_libraries(bu_gtk 
        ${GTK3_LIBRARIES}
    )
endif()