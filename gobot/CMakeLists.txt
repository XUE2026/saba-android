cmake_minimum_required(VERSION 3.2)

project(gobot)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(
    GOBOT_HEADER
    include/gobot/gobot.h
    include/gobot/GobotTypes.h
    include/gobot/GobotFileIO.h
    include/gobot/GobotConverter.h
)

set(
    GOBOT_SOURCE
    src/GobotFileIO.cpp
    src/GobotConverter.cpp
)

set(GOBOT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(gobot STATIC
    ${GOBOT_SOURCE}
    ${GOBOT_HEADER}
)

target_include_directories(gobot
    PUBLIC ${GOBOT_INCLUDE_DIR}
)

target_compile_features(gobot PUBLIC cxx_std_14)