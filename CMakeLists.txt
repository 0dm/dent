cmake_minimum_required(VERSION 3.17)
project(dent)
if (WIN32)
    add_subdirectory(External/mingw-std-threads)
endif ()
set(src
        CPU/CPU.cpp
        CPU/CPU.h

        GFX/GFX.h
        GFX/GFX.cpp)

set(CMAKE_CXX_FLAGS_DEBUG  "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE  "-O3")

add_executable(dent ${src} main.cpp)
target_link_libraries(dent mingw32 SDL2main SDL2 )