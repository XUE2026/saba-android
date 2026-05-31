cmake_minimum_required(VERSION 3.22)
project(saba_android)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ANDROID_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# External dependencies from the workspace
set(SABA_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../src)
set(SABA_VIEWER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../viewer)
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../external)

# Include directories
include_directories(
    ${SABA_SOURCE_DIR}
    ${SABA_VIEWER_DIR}
    ${EXTERNAL_DIR}/gl3w/include
    ${EXTERNAL_DIR}/gli/include
    ${EXTERNAL_DIR}/glm/include
    ${EXTERNAL_DIR}/stb/include
    ${EXTERNAL_DIR}/spdlog/include
    ${EXTERNAL_DIR}/tinyobjloader/include
    ${EXTERNAL_DIR}/imgui/include
    ${EXTERNAL_DIR}/json/include
    ${EXTERNAL_DIR}/lua
    ${EXTERNAL_DIR}/sol2/include
    ${EXTERNAL_DIR}/tinyxfileloader/include
    ${EXTERNAL_DIR}/tinyddsloader/include
    ${EXTERNAL_DIR}/fcpp
    ${ANDROID_SOURCE_DIR}
)

# Android-specific macro
add_definitions(-DSABA_ANDROID=1)
add_definitions(-DGLES_ENABLED=1)

# Saba Base library sources
set(SABA_BASE_SOURCES
    ${SABA_SOURCE_DIR}/Saba/Base/File.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/Log.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/Path.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/Singleton.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/Time.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/UnicodeUtil.cpp
)

# Saba Model sources
set(SABA_MODEL_SOURCES
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
    ${SABA_SOURCE_DIR}/Saba/Model/OBJ/OBJModel.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/OBJ/tinyobjloader.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/XFile/XFileModel.cpp
)

# GL Renderer sources (adapted for GLES)
set(SABA_GL_SOURCES
    ${SABA_VIEWER_DIR}/Saba/GL/GLSLUtil.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/GLShaderUtil.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/GLTextureUtil.cpp
)

# GL Model sources
set(SABA_GL_MODEL_MMD_SOURCES
    ${SABA_VIEWER_DIR}/Saba/GL/Model/MMD/GLMMDModel.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/Model/MMD/GLMMDModelDrawContext.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/Model/MMD/GLMMDModelDrawer.cpp
)

set(SABA_GL_MODEL_OBJ_SOURCES
    ${SABA_VIEWER_DIR}/Saba/GL/Model/OBJ/GLOBJModel.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/Model/OBJ/GLOBJModelDrawContext.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/Model/OBJ/GLOBJModelDrawer.cpp
)

set(SABA_GL_MODEL_XFILE_SOURCES
    ${SABA_VIEWER_DIR}/Saba/GL/Model/XFile/GLXFileModel.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/Model/XFile/GLXFileModelDrawContext.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/Model/XFile/GLXFileModelDrawer.cpp
)

# Viewer sources
set(SABA_VIEWER_SOURCES
    ${SABA_VIEWER_DIR}/Saba/Viewer/Camera.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/Light.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/Grid.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/ModelDrawer.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/ViewerContext.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/CameraOverrider.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/VMDCameraOverrider.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/ShadowMap.cpp
)

# Android-specific sources
set(SABA_ANDROID_SOURCES
    egl_core.cpp
    jni_bridge.cpp
    android_viewer.cpp
    filter_system.cpp
    environment_system.cpp
    action_recorder.cpp
    bone_controller.cpp
    gobot_framework.cpp
)

# gl3w (for GLES this needs adaptation)
set(GL3W_SOURCE ${EXTERNAL_DIR}/gl3w/src/gl3w.c)

# ImGui sources
set(IMGUI_SOURCES
    ${EXTERNAL_DIR}/imgui/src/imgui.cpp
    ${EXTERNAL_DIR}/imgui/src/imgui_draw.cpp
    ${EXTERNAL_DIR}/imgui/src/imgui_widgets.cpp
    ${EXTERNAL_DIR}/imgui/src/ImGuizmo.cpp
)

set(FC_PP_SOURCES
    ${EXTERNAL_DIR}/fcpp/cpp1.c
    ${EXTERNAL_DIR}/fcpp/cpp2.c
    ${EXTERNAL_DIR}/fcpp/cpp3.c
    ${EXTERNAL_DIR}/fcpp/cpp4.c
    ${EXTERNAL_DIR}/fcpp/cpp5.c
    ${EXTERNAL_DIR}/fcpp/cpp6.c
)

# Lua sources
set(LUA_SOURCES
    ${EXTERNAL_DIR}/lua/lapi.c
    ${EXTERNAL_DIR}/lua/lauxlib.c
    ${EXTERNAL_DIR}/lua/lbaselib.c
    ${EXTERNAL_DIR}/lua/lbitlib.c
    ${EXTERNAL_DIR}/lua/lcode.c
    ${EXTERNAL_DIR}/lua/lcorolib.c
    ${EXTERNAL_DIR}/lua/lctype.c
    ${EXTERNAL_DIR}/lua/ldblib.c
    ${EXTERNAL_DIR}/lua/ldebug.c
    ${EXTERNAL_DIR}/lua/ldo.c
    ${EXTERNAL_DIR}/lua/ldump.c
    ${EXTERNAL_DIR}/lua/lfunc.c
    ${EXTERNAL_DIR}/lua/lgc.c
    ${EXTERNAL_DIR}/lua/linit.c
    ${EXTERNAL_DIR}/lua/liolib.c
    ${EXTERNAL_DIR}/lua/llex.c
    ${EXTERNAL_DIR}/lua/lmathlib.c
    ${EXTERNAL_DIR}/lua/lmem.c
    ${EXTERNAL_DIR}/lua/loadlib.c
    ${EXTERNAL_DIR}/lua/lobject.c
    ${EXTERNAL_DIR}/lua/lopcodes.c
    ${EXTERNAL_DIR}/lua/loslib.c
    ${EXTERNAL_DIR}/lua/lparser.c
    ${EXTERNAL_DIR}/lua/lstate.c
    ${EXTERNAL_DIR}/lua/lstring.c
    ${EXTERNAL_DIR}/lua/lstrlib.c
    ${EXTERNAL_DIR}/lua/ltable.c
    ${EXTERNAL_DIR}/lua/ltablib.c
    ${EXTERNAL_DIR}/lua/ltm.c
    ${EXTERNAL_DIR}/lua/lundump.c
    ${EXTERNAL_DIR}/lua/lutf8lib.c
    ${EXTERNAL_DIR}/lua/lvm.c
    ${EXTERNAL_DIR}/lua/lzio.c
)

# Create the shared library
add_library(SabaViewer SHARED
    ${SABA_BASE_SOURCES}
    ${SABA_MODEL_SOURCES}
    ${SABA_GL_SOURCES}
    ${SABA_GL_MODEL_MMD_SOURCES}
    ${SABA_GL_MODEL_OBJ_SOURCES}
    ${SABA_GL_MODEL_XFILE_SOURCES}
    ${SABA_VIEWER_SOURCES}
    ${SABA_ANDROID_SOURCES}
    ${IMGUI_SOURCES}
    ${FC_PP_SOURCES}
    ${LUA_SOURCES}
)

# For GLES we don't need gl3w on Android
target_include_directories(SabaViewer PRIVATE
    ${EXTERNAL_DIR}/imgui/src
    ${EXTERNAL_DIR}/fcpp
)

# Link Android libraries
target_link_libraries(SabaViewer
    GLESv3
    EGL
    android
    log
    z
)

# OpenGL ES specific definitions
target_compile_definitions(SabaViewer PRIVATE
    GLES_ENABLED
    GLM_FORCE_RADIANS
    GLM_FORCE_DEPTH_ZERO_TO_ONE
)