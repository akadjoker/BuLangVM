project(game)
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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)





file(GLOB SOURCES "src/*.cpp")
add_executable(game   ${SOURCES})


target_include_directories(libbu PUBLIC  include src)



if(CMAKE_BUILD_TYPE MATCHES Debug)

if (UNIX)
target_compile_options(game PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g  -D_DEBUG -DVERBOSE)
target_link_options(game PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g  -D_DEBUG) 
endif()

# if (UNIX)
#     target_compile_options(game PRIVATE
#         -O3
#         -g
#         -fno-omit-frame-pointer
#     )
# endif()


elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(game PRIVATE -O3   -DNDEBUG -s)
    target_link_options(game PRIVATE -O3   -DNDEBUG )
endif()

target_link_libraries(game libbu raylib)

if (WIN32)
    target_link_libraries(game Winmm.lib)
endif()


if (UNIX)
    target_link_libraries(game  m )
endif()
