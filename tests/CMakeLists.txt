project(tests)
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

add_compile_options(
        # Optimization level
        -O3
        
       
        
        # Architecture specific
        -march=native
        -mtune=native
        

        
        # Vectorization
        -ftree-vectorize
        
        # Strip debug info
        -DNDEBUG
        
        # Inline agressivo
        -finline-functions
        -funroll-loops

)

 

file(GLOB SOURCES "src/*.cpp")
add_executable(tests   ${SOURCES})


target_include_directories(libwdiv PUBLIC  include src)



if(CMAKE_BUILD_TYPE MATCHES Debug)

if (UNIX)
target_compile_options(tests PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g  -D_DEBUG -DVERBOSE)
target_link_options(tests PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -g  -D_DEBUG) 
endif()
 
   
endif()

target_link_libraries(tests libwdiv)

if (WIN32)
    target_link_libraries(tests Winmm.lib)
endif()


if (UNIX)
    target_link_libraries(tests  m )
endif()
