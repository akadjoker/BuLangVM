cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0072 NEW)

project(bulang VERSION 2.2.1)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
 

find_program(CLANG_TIDY_EXE NAMES clang-tidy)

if(CLANG_TIDY_EXE)
    add_custom_target(
        tidy
        COMMAND ${CLANG_TIDY_EXE}
        -p=${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/libbu/src/*.cpp
        ${CMAKE_SOURCE_DIR}/main/src/*.cpp
        COMMENT "Running clang-tidy"
    )
endif()

 
add_subdirectory(vendor/miniz)
if(TARGET miniz)
    set_target_properties(miniz PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
 
 
 
 

add_subdirectory(libbu)

add_subdirectory(bulang)
 
# Language test suite (lightweight: only needs libbu)
enable_testing()
add_subdirectory(tests)


 
