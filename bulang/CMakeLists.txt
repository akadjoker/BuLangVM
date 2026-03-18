project(bulang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(bulang src/main.cpp)

target_include_directories(bulang PRIVATE ${CMAKE_SOURCE_DIR}/libbu/include)

# Platform-specific compiler flags
if(MSVC)
    # Windows MSVC
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        target_compile_options(bulang PRIVATE /Zi /Od /D_DEBUG)
    elseif(CMAKE_BUILD_TYPE MATCHES Release)
        target_compile_options(bulang PRIVATE /O2 /DNDEBUG)
    endif()
else()
    # GCC/Clang (Linux, macOS, MinGW)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        target_compile_options(bulang PRIVATE -g -D_DEBUG)
        if(UNIX)
            target_compile_options(bulang PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak)
            target_link_options(bulang PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g -rdynamic)
        endif()
    elseif(CMAKE_BUILD_TYPE MATCHES Release)
        target_compile_options(bulang PRIVATE -O3 -DNDEBUG)
        if(UNIX)
            target_link_options(bulang PRIVATE -s -rdynamic)
        endif()
    endif()
endif()

# Export symbols so plugins can use them
if(UNIX)
    target_link_options(bulang PRIVATE -rdynamic)
endif()

target_link_libraries(bulang libbu)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(bulang Winmm.lib ws2_32)
endif()

if(UNIX)
    target_link_libraries(bulang m dl)
endif()