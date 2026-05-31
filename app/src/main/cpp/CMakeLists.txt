cmake_minimum_required(VERSION 3.22.1)

project("saba_native")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ============================================================
# Project root directory (three levels up from this CMakeLists.txt)
# ============================================================
get_filename_component(SABA_PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../ REALPATH)
get_filename_component(SABA_SOURCE_DIR ${SABA_PROJECT_ROOT}/src REALPATH)
get_filename_component(SABA_VIEWER_DIR ${SABA_PROJECT_ROOT}/viewer REALPATH)
get_filename_component(SABA_EXTERNAL_DIR ${SABA_PROJECT_ROOT}/external REALPATH)

message(STATUS "SABA_PROJECT_ROOT: ${SABA_PROJECT_ROOT}")
message(STATUS "SABA_SOURCE_DIR: ${SABA_SOURCE_DIR}")
message(STATUS "SABA_VIEWER_DIR: ${SABA_VIEWER_DIR}")

# ============================================================
# Include directories
# ============================================================
include_directories(${SABA_SOURCE_DIR})
include_directories(${SABA_VIEWER_DIR})
include_directories(${SABA_EXTERNAL_DIR}/gl3w/include)
include_directories(${SABA_EXTERNAL_DIR}/gli/include)
include_directories(${SABA_EXTERNAL_DIR}/glm/include)
include_directories(${SABA_EXTERNAL_DIR}/stb/include)
include_directories(${SABA_EXTERNAL_DIR}/spdlog/include)
include_directories(${SABA_EXTERNAL_DIR}/tinyobjloader/include)
include_directories(${SABA_EXTERNAL_DIR}/imgui/include)
include_directories(${SABA_EXTERNAL_DIR}/json/include)
include_directories(BEFORE ${SABA_EXTERNAL_DIR}/lua)
include_directories(${SABA_EXTERNAL_DIR}/sol2/include)
include_directories(${SABA_EXTERNAL_DIR}/tinyxfileloader/include)
include_directories(${SABA_EXTERNAL_DIR}/tinyddsloader/include)
include_directories(${SABA_EXTERNAL_DIR}/fcpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ============================================================
# Bullet Physics (system or Android cross-compiled)
# ============================================================
if(DEFINED ENV{BULLET_ANDROID_DIR})
    set(BULLET_ROOT "$ENV{BULLET_ANDROID_DIR}/${ANDROID_ABI}")
    message(STATUS "Using Android Bullet Physics at: ${BULLET_ROOT}")
    include_directories(${BULLET_ROOT}/include/bullet)
    link_directories(${BULLET_ROOT}/lib)
    set(BULLET_LIBRARIES
        BulletDynamics
        BulletCollision
        LinearMath
    )
else()
    find_package(Bullet QUIET)
    if(BULLET_FOUND)
        message(STATUS "Using system Bullet: ${BULLET_INCLUDE_DIRS}")
        include_directories(${BULLET_INCLUDE_DIRS})
        set(BULLET_LIBRARIES ${BULLET_LIBRARIES})
    else()
        message(STATUS "Bullet not found, checking /usr/include/bullet")
        include_directories(/usr/include/bullet)
        set(BULLET_LIBRARIES
            BulletDynamics
            BulletCollision
            LinearMath
        )
    endif()
endif()

# ============================================================
# Compile definitions
# ============================================================
add_definitions(-DANDROID)
add_definitions(-DSABA_ANDROID)

# ============================================================
# Saba Core Library Sources (from src/)
# ============================================================
set(SABA_CORE_SOURCES
    ${SABA_SOURCE_DIR}/Saba/Base/File.h
    ${SABA_SOURCE_DIR}/Saba/Base/File.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/Log.h
    ${SABA_SOURCE_DIR}/Saba/Base/Log.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/Path.h
    ${SABA_SOURCE_DIR}/Saba/Base/Path.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/Singleton.h
    ${SABA_SOURCE_DIR}/Saba/Base/Singleton.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/Time.h
    ${SABA_SOURCE_DIR}/Saba/Base/Time.cpp
    ${SABA_SOURCE_DIR}/Saba/Base/UnicodeUtil.h
    ${SABA_SOURCE_DIR}/Saba/Base/UnicodeUtil.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/OBJ/OBJModel.h
    ${SABA_SOURCE_DIR}/Saba/Model/OBJ/OBJModel.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/OBJ/tinyobjloader.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/XFile/XFileModel.h
    ${SABA_SOURCE_DIR}/Saba/Model/XFile/XFileModel.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDCamera.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDCamera.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDFileString.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDIkSolver.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDIkSolver.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDMaterial.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDMaterial.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDModel.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDModel.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDMorph.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDMorph.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDNode.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDNode.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDPhysics.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/MMDPhysics.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/PMDFile.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/PMDFile.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/PMDModel.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/PMDModel.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/PMXFile.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/PMXFile.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/PMXModel.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/PMXModel.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/SjisToUnicode.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/SjisToUnicode.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/VMDAnimation.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/VMDAnimation.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/VMDAnimationCommon.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/VMDCameraAnimation.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/VMDCameraAnimation.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/VMDFile.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/VMDFile.cpp
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/VPDFile.h
    ${SABA_SOURCE_DIR}/Saba/Model/MMD/VPDFile.cpp
)

# ============================================================
# Saba Viewer GL Sources (from viewer/)
# ============================================================
set(SABA_VIEWER_GL_SOURCES
    ${SABA_VIEWER_DIR}/Saba/GL/GLObject.h
    ${SABA_VIEWER_DIR}/Saba/GL/GLShaderUtil.h
    ${SABA_VIEWER_DIR}/Saba/GL/GLShaderUtil.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/GLSLUtil.h
    ${SABA_VIEWER_DIR}/Saba/GL/GLSLUtil.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/GLTextureUtil.h
    ${SABA_VIEWER_DIR}/Saba/GL/GLTextureUtil.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/GLVertexUtil.h
    ${SABA_VIEWER_DIR}/Saba/GL/Model/MMD/GLMMDModel.h
    ${SABA_VIEWER_DIR}/Saba/GL/Model/MMD/GLMMDModel.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/Model/MMD/GLMMDModelDrawContext.h
    ${SABA_VIEWER_DIR}/Saba/GL/Model/MMD/GLMMDModelDrawContext.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/Model/MMD/GLMMDModelDrawer.h
    ${SABA_VIEWER_DIR}/Saba/GL/Model/MMD/GLMMDModelDrawer.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/Model/OBJ/GLOBJModel.h
    ${SABA_VIEWER_DIR}/Saba/GL/Model/OBJ/GLOBJModel.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/Model/OBJ/GLOBJModelDrawContext.h
    ${SABA_VIEWER_DIR}/Saba/GL/Model/OBJ/GLOBJModelDrawContext.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/Model/OBJ/GLOBJModelDrawer.h
    ${SABA_VIEWER_DIR}/Saba/GL/Model/OBJ/GLOBJModelDrawer.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/Model/XFile/GLXFileModel.h
    ${SABA_VIEWER_DIR}/Saba/GL/Model/XFile/GLXFileModel.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/Model/XFile/GLXFileModelDrawContext.h
    ${SABA_VIEWER_DIR}/Saba/GL/Model/XFile/GLXFileModelDrawContext.cpp
    ${SABA_VIEWER_DIR}/Saba/GL/Model/XFile/GLXFileModelDrawer.h
    ${SABA_VIEWER_DIR}/Saba/GL/Model/XFile/GLXFileModelDrawer.cpp
)

# ============================================================
# Saba Viewer Core Sources (from viewer/)
# Note: Viewer.cpp depends on GLFW, which is not available on Android.
# An Android-specific Viewer replacement should be used instead.
# ============================================================
set(SABA_VIEWER_CORE_SOURCES
    ${SABA_VIEWER_DIR}/Saba/Viewer/Camera.h
    ${SABA_VIEWER_DIR}/Saba/Viewer/Camera.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/CameraOverrider.h
    ${SABA_VIEWER_DIR}/Saba/Viewer/CameraOverrider.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/Grid.h
    ${SABA_VIEWER_DIR}/Saba/Viewer/Grid.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/Light.h
    ${SABA_VIEWER_DIR}/Saba/Viewer/Light.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/ModelDrawer.h
    ${SABA_VIEWER_DIR}/Saba/Viewer/ModelDrawer.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/ShadowMap.h
    ${SABA_VIEWER_DIR}/Saba/Viewer/ShadowMap.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/VMDCameraOverrider.h
    ${SABA_VIEWER_DIR}/Saba/Viewer/VMDCameraOverrider.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/Viewer.h
    ${SABA_VIEWER_DIR}/Saba/Viewer/ViewerContext.h
    ${SABA_VIEWER_DIR}/Saba/Viewer/ViewerContext.cpp
    ${SABA_VIEWER_DIR}/Saba/Viewer/ViewerCommand.h
    ${SABA_VIEWER_DIR}/Saba/Viewer/ViewerCommand.cpp
)

# Note: Viewer.cpp is intentionally excluded because it depends on GLFW.
# Add it back once an Android-specific GLFW replacement is available.
# ${SABA_VIEWER_DIR}/Saba/Viewer/Viewer.cpp

# ============================================================
# External Libraries
# ============================================================

# fcpp (C preprocessor for shader includes)
set(FCPP_SOURCES
    ${SABA_EXTERNAL_DIR}/fcpp/cpp1.c
    ${SABA_EXTERNAL_DIR}/fcpp/cpp2.c
    ${SABA_EXTERNAL_DIR}/fcpp/cpp3.c
    ${SABA_EXTERNAL_DIR}/fcpp/cpp4.c
    ${SABA_EXTERNAL_DIR}/fcpp/cpp5.c
    ${SABA_EXTERNAL_DIR}/fcpp/cpp6.c
)

# Lua
set(LUA_SOURCES
    ${SABA_EXTERNAL_DIR}/lua/lapi.c
    ${SABA_EXTERNAL_DIR}/lua/lauxlib.c
    ${SABA_EXTERNAL_DIR}/lua/lbaselib.c
    ${SABA_EXTERNAL_DIR}/lua/lbitlib.c
    ${SABA_EXTERNAL_DIR}/lua/lcode.c
    ${SABA_EXTERNAL_DIR}/lua/lcorolib.c
    ${SABA_EXTERNAL_DIR}/lua/lctype.c
    ${SABA_EXTERNAL_DIR}/lua/ldblib.c
    ${SABA_EXTERNAL_DIR}/lua/ldebug.c
    ${SABA_EXTERNAL_DIR}/lua/ldo.c
    ${SABA_EXTERNAL_DIR}/lua/ldump.c
    ${SABA_EXTERNAL_DIR}/lua/lfunc.c
    ${SABA_EXTERNAL_DIR}/lua/lgc.c
    ${SABA_EXTERNAL_DIR}/lua/linit.c
    ${SABA_EXTERNAL_DIR}/lua/liolib.c
    ${SABA_EXTERNAL_DIR}/lua/llex.c
    ${SABA_EXTERNAL_DIR}/lua/lmathlib.c
    ${SABA_EXTERNAL_DIR}/lua/lmem.c
    ${SABA_EXTERNAL_DIR}/lua/loadlib.c
    ${SABA_EXTERNAL_DIR}/lua/lobject.c
    ${SABA_EXTERNAL_DIR}/lua/lopcodes.c
    ${SABA_EXTERNAL_DIR}/lua/loslib.c
    ${SABA_EXTERNAL_DIR}/lua/lparser.c
    ${SABA_EXTERNAL_DIR}/lua/lstate.c
    ${SABA_EXTERNAL_DIR}/lua/lstring.c
    ${SABA_EXTERNAL_DIR}/lua/lstrlib.c
    ${SABA_EXTERNAL_DIR}/lua/ltable.c
    ${SABA_EXTERNAL_DIR}/lua/ltablib.c
    ${SABA_EXTERNAL_DIR}/lua/ltm.c
    ${SABA_EXTERNAL_DIR}/lua/lundump.c
    ${SABA_EXTERNAL_DIR}/lua/lutf8lib.c
    ${SABA_EXTERNAL_DIR}/lua/lvm.c
    ${SABA_EXTERNAL_DIR}/lua/lzio.c
)

# imgui (excluding platform-specific backends)
set(IMGUI_SOURCES
    ${SABA_EXTERNAL_DIR}/imgui/src/imgui.cpp
    ${SABA_EXTERNAL_DIR}/imgui/src/imgui_draw.cpp
    ${SABA_EXTERNAL_DIR}/imgui/src/imgui_widgets.cpp
    ${SABA_EXTERNAL_DIR}/imgui/src/ImGuizmo.cpp
)

# gl3w - Not used on Android; Android NDK provides GLES3 headers natively.
# Use <GLES3/gl3.h> and <GLES3/gl3ext.h> instead.

# ============================================================
# Native bridge (Android-specific JNI entry point)
# ============================================================
set(NATIVE_BRIDGE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/native_bridge.cpp
)

# ============================================================
# Build target
# ============================================================
add_library(
    saba_native
    SHARED
    ${NATIVE_BRIDGE_SOURCES}
    ${SABA_CORE_SOURCES}
    ${SABA_VIEWER_GL_SOURCES}
    ${SABA_VIEWER_CORE_SOURCES}
    ${FCPP_SOURCES}
    ${LUA_SOURCES}
    ${IMGUI_SOURCES}
)

# ============================================================
# Link libraries
# ============================================================
target_link_libraries(
    saba_native
    android
    EGL
    GLESv3
    log
    z
    ${BULLET_LIBRARIES}
)

# ============================================================
# Asset copy for shader resources
# ============================================================
set(SHADER_RESOURCE_DIR ${SABA_VIEWER_DIR}/Saba/Viewer/resource/shader)
if(EXISTS ${SHADER_RESOURCE_DIR})
    add_custom_command(TARGET saba_native POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SHADER_RESOURCE_DIR}
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/resource/shader
    )
endif()