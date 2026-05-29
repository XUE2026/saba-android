//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef GOBOT_GOBOTTYPES_H_
#define GOBOT_GOBOTTYPES_H_

#include "gobot.h"

#include <string>
#include <vector>
#include <cstdint>

namespace gobot
{
    //=============================================================================
    // Helper structs
    //=============================================================================

    struct GobotVec3
    {
        float m_x;
        float m_y;
        float m_z;

        GobotVec3() : m_x(0.0f), m_y(0.0f), m_z(0.0f) {}
        GobotVec3(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {}
    };

    struct GobotQuat
    {
        float m_x;
        float m_y;
        float m_z;
        float m_w;

        GobotQuat() : m_x(0.0f), m_y(0.0f), m_z(0.0f), m_w(1.0f) {}
        GobotQuat(float x, float y, float z, float w) : m_x(x), m_y(y), m_z(z), m_w(w) {}
    };

    struct GobotColor
    {
        float m_r;
        float m_g;
        float m_b;
        float m_a;

        GobotColor() : m_r(1.0f), m_g(1.0f), m_b(1.0f), m_a(1.0f) {}
        GobotColor(float r, float g, float b, float a) : m_r(r), m_g(g), m_b(b), m_a(a) {}
    };

    //=============================================================================
    // Action File (.gobot_action)
    //=============================================================================

    struct GobotActionHeader
    {
        char        m_magic[8];
        uint32_t    m_version;
        uint32_t    m_boneCount;
        uint32_t    m_frameCount;
        float       m_duration;
    };

    struct GobotBoneInfo
    {
        std::string m_name;
        int32_t     m_parentIndex;
    };

    struct GobotActionFrame
    {
        float       m_time;
        uint32_t    m_boneIndex;
        GobotVec3   m_position;
        GobotQuat   m_rotation;
    };

    struct GobotActionFile
    {
        GobotActionHeader               m_header;
        std::vector<GobotBoneInfo>      m_bones;
        std::vector<GobotActionFrame>   m_frames;

        GobotActionFile()
        {
            m_header.m_magic[0] = 'G'; m_header.m_magic[1] = 'O';
            m_header.m_magic[2] = 'B'; m_header.m_magic[3] = 'O';
            m_header.m_magic[4] = 'T'; m_header.m_magic[5] = '_';
            m_header.m_magic[6] = 'A'; m_header.m_magic[7] = 'C';
            m_header.m_version = GOBOT_ACTION_VERSION;
            m_header.m_boneCount = 0;
            m_header.m_frameCount = 0;
            m_header.m_duration = 0.0f;
        }
    };

    //=============================================================================
    // Scene File (.gobot_scene)
    //=============================================================================

    struct GobotSceneHeader
    {
        char        m_magic[8];
        uint32_t    m_version;
    };

    struct GobotSceneCamera
    {
        GobotVec3   m_position;
        GobotVec3   m_target;
        float       m_fov;
        float       m_nearClip;
        float       m_farClip;

        GobotSceneCamera()
            : m_fov(45.0f)
            , m_nearClip(0.1f)
            , m_farClip(1000.0f)
        {
        }
    };

    struct GobotSceneLight
    {
        GobotLightType  m_type;
        GobotVec3       m_position;
        GobotVec3       m_direction;
        GobotColor      m_color;
        float           m_intensity;

        GobotSceneLight()
            : m_type(GobotLightType::Directional)
            , m_intensity(1.0f)
        {
        }
    };

    struct GobotSceneEnvironment
    {
        GobotGroundType m_groundType;
        GobotColor      m_ambientColor;
        GobotColor      m_fogColor;
        float           m_fogDensity;

        GobotSceneEnvironment()
            : m_groundType(GobotGroundType::Floor)
            , m_ambientColor(0.2f, 0.2f, 0.2f, 1.0f)
            , m_fogColor(0.5f, 0.5f, 0.5f, 1.0f)
            , m_fogDensity(0.0f)
        {
        }
    };

    struct GobotScenePrimitive
    {
        GobotPrimitiveType  m_type;
        GobotVec3           m_size;
        GobotVec3           m_position;
        GobotVec3           m_rotation;
        GobotColor          m_color;

        GobotScenePrimitive()
            : m_type(GobotPrimitiveType::Cube)
            , m_size(1.0f, 1.0f, 1.0f)
        {
        }
    };

    struct GobotSceneFilter
    {
        GobotFilterType m_filterType;
        float           m_exposure;
        float           m_contrast;
        float           m_saturation;
        float           m_hueShift;
        float           m_temperature;
        float           m_tint;

        GobotSceneFilter()
            : m_filterType(GobotFilterType::None)
            , m_exposure(1.0f)
            , m_contrast(1.0f)
            , m_saturation(1.0f)
            , m_hueShift(0.0f)
            , m_temperature(0.0f)
            , m_tint(0.0f)
        {
        }
    };

    struct GobotSceneFile
    {
        GobotSceneHeader                m_header;
        GobotSceneCamera                m_camera;
        std::vector<GobotSceneLight>    m_lights;
        GobotSceneEnvironment           m_environment;
        std::vector<GobotScenePrimitive> m_primitives;
        GobotSceneFilter                m_filter;

        GobotSceneFile()
        {
            m_header.m_magic[0] = 'G'; m_header.m_magic[1] = 'O';
            m_header.m_magic[2] = 'B'; m_header.m_magic[3] = 'O';
            m_header.m_magic[4] = 'T'; m_header.m_magic[5] = '_';
            m_header.m_magic[6] = 'S'; m_header.m_magic[7] = 'C';
            m_header.m_version = GOBOT_SCENE_VERSION;
        }
    };

    //=============================================================================
    // Project File (.gobot_proj)
    //=============================================================================

    struct GobotProjectHeader
    {
        char        m_magic[8];
        uint32_t    m_version;
    };

    struct GobotUIWindow
    {
        std::string m_name;
        float       m_x;
        float       m_y;
        float       m_width;
        float       m_height;
    };

    struct GobotProjectFile
    {
        GobotProjectHeader              m_header;
        std::vector<std::string>        m_modelPaths;
        std::vector<std::string>        m_actionPaths;
        GobotSceneFile                  m_scene;
        bool                            m_hasUILayout;
        std::vector<GobotUIWindow>      m_uiWindows;
        std::string                     m_language;
        GobotOrientation                m_orientation;

        GobotProjectFile()
            : m_hasUILayout(false)
            , m_orientation(GobotOrientation::Landscape)
        {
            m_header.m_magic[0] = 'G'; m_header.m_magic[1] = 'O';
            m_header.m_magic[2] = 'B'; m_header.m_magic[3] = 'O';
            m_header.m_magic[4] = 'T'; m_header.m_magic[5] = '_';
            m_header.m_magic[6] = 'P'; m_header.m_magic[7] = 'R';
            m_header.m_version = GOBOT_PROJECT_VERSION;
        }
    };

} // namespace gobot

#endif // !GOBOT_GOBOTTYPES_H_