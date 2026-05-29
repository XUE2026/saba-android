cmake_minimum_required(VERSION 3.22.1)

project("saba_native")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

get_filename_component(SABA_PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../.. REALPATH)
get_filename_component(SABA_SOURCE_DIR ${SABA_PROJECT_ROOT}/src REALPATH)
get_filename_component(SABA_VIEWER_DIR ${SABA_PROJECT_ROOT}/viewer REALPATH)
get_filename_component(SABA_EXTERNAL_DIR ${SABA_PROJECT_ROOT}/external REALPATH)

message(STATUS "SABA_PROJECT_ROOT: ${SABA_PROJECT_ROOT}")
message(STATUS "SABA_SOURCE_DIR: ${SABA_SOURCE_DIR}")

include_directories(${SABA_SOURCE_DIR})
include_directories(${SABA_VIEWER_DIR})
include_directories(${SABA_EXTERNAL_DIR}/gli/include)
include_directories(${SABA_EXTERNAL_DIR}/glm/include)
include_directories(${SABA_EXTERNAL_DIR}/stb/include)
include_directories(${SABA_EXTERNAL_DIR}/spdlog/include)
include_directories(${SABA_EXTERNAL_DIR}/tinyobjloader/include)
include_directories(${SABA_EXTERNAL_DIR}/json/include)
include_directories(BEFORE ${SABA_EXTERNAL_DIR}/lua)
include_directories(${SABA_EXTERNAL_DIR}/sol2/include)
include_directories(${SABA_EXTERNAL_DIR}/tinyxfileloader/include)
include_directories(${SABA_EXTERNAL_DIR}/tinyddsloader/include)
include_directories(${SABA_EXTERNAL_DIR}/fcpp)
include_directories(${SABA_EXTERNAL_DIR}/gobot/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-DANDROID)
add_definitions(-DSABA_ANDROID)

set(GLM_ENABLE_EXPERIMENTAL)

set(SABA_CORE_SOURCES
    ${SABA_SOURCE_DIR}/Saba/Base/File.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/Log.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/Path.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/Singleton.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/Time.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/UnicodeUtil.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDCamera.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDIkSolver.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDMaterial.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDModel.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDMorph.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDNode.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/PMDFile.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/PMDModel.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/PMXFile.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/PMXModel.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/SjisToUnicode.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/VMDAnimation.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/VMDCameraAnimation.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/VMDFile.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/VPDFile.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/TextureManager.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/ModelCache.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/OBJ/OBJModel.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/OBJ/tinyobjloader.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/XFile/XFileModel.cpp
)

set(FCPP_SOURCES
    ${SABA_EXTERNAL_DIR}/fcpp/cpp1.c
    ${SABA_EXTERNAL_DIR}/fcpp/cpp2.c
    ${SABA_EXTERNAL_DIR}/fcpp/cpp3.c
    ${SABA_EXTERNAL_DIR}/fcpp/cpp4.c
    ${SABA_EXTERNAL_DIR}/fcpp/cpp5.c
    ${SABA_EXTERNAL_DIR}/fcpp/cpp6.c
)

set(GOBOT_SOURCES
    ${SABA_PROJECT_ROOT}/gobot/src/GobotFileIO.cpp
    ${SABA_PROJECT_ROOT}/gobot/src/GobotConverter.cpp
)

set(NATIVE_BRIDGE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/native_bridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/android_renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/android_model_loader.cpp
)

add_library(
    saba_native
    SHARED
    ${NATIVE_BRIDGE_SOURCES}
    ${SABA_CORE_SOURCES}
    ${FCPP_SOURCES}
    ${GOBOT_SOURCES}
)

target_link_libraries(
    saba_native
    android
    EGL
    GLESv3
    log
    z
)