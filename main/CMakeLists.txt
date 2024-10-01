project(main)
cmake_policy(SET CMP0072 NEW)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Dfsanitize=address -D_CRT_SECURE_NO_WARNINGS")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_compile_options(
    -Wall
)


file(GLOB SOURCES "src/*.cpp")
add_executable(main   ${SOURCES})


target_include_directories(main PUBLIC  include src)



if(CMAKE_BUILD_TYPE MATCHES Debug)

if (UNIX)
target_compile_options(main PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g  -D_DEBUG)
target_link_options(main PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g  -D_DEBUG) 
endif()


elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(main PRIVATE -O3   -DNDEBUG)
    target_link_options(main PRIVATE -O3   -DNDEBUG)
endif()

target_link_libraries(main bulang )

if (WIN32)
    target_link_libraries(main Winmm.lib)
endif()


if (UNIX)
    target_link_libraries(main  m )
endif()
