project(bu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(bu src/main.cpp)

target_include_directories(bu PRIVATE ${CMAKE_SOURCE_DIR}/libbu/include)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    if (UNIX)
        target_compile_options(bu PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g -D_DEBUG)
        target_link_options(bu PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g -D_DEBUG -rdynamic)
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(bu PRIVATE -O3 -DNDEBUG -s)
    target_link_options(bu PRIVATE -O3 -DNDEBUG -rdynamic)
endif()

# Export symbols so plugins can use them
if (UNIX)
    target_link_options(bu PRIVATE -rdynamic)
endif()

target_link_libraries(bu libbu)

if (WIN32)
    target_link_libraries(bu Winmm.lib)
endif()

if (UNIX)
    target_link_libraries(bu m dl)
endif()
