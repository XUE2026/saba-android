cmake_minimum_required(VERSION 3.10)
project(gobot VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(gobot STATIC
    src/GobotFileIO.cpp
    src/GobotConverter.cpp
)

target_include_directories(gobot PUBLIC include)
target_include_directories(gobot PRIVATE src)