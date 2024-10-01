
project(bulang)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Dfsanitize=address -D_CRT_SECURE_NO_WARNINGS")
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)




set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)



file(GLOB SOURCES "src/*.cpp")
add_library(bulang STATIC  ${SOURCES})


target_include_directories(bulang PUBLIC include include/std src)
target_precompile_headers(bulang PRIVATE src/pch.h)


