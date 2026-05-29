//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include <gobot/GobotFileIO.h>

#include <fstream>
#include <cstring>

namespace gobot
{
    GobotStatus GobotFileIO::s_lastStatus = GobotStatus::Success;

    //=============================================================================
    // Write helpers
    //=============================================================================

    bool GobotFileIO::WriteUint8(std::ofstream& stream, uint8_t value)
    {
        stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
        return stream.good();
    }

    bool GobotFileIO::WriteUint32(std::ofstream& stream, uint32_t value)
    {
        stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
        return stream.good();
    }

    bool GobotFileIO::WriteFloat(std::ofstream& stream, float value)
    {
        stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
        return stream.good();
    }

    bool GobotFileIO::WriteVec3(std::ofstream& stream, const GobotVec3& vec)
    {
        if (!WriteFloat(stream, vec.m_x)) return false;
        if (!WriteFloat(stream, vec.m_y)) return false;
        if (!WriteFloat(stream, vec.m_z)) return false;
        return true;
    }

    bool GobotFileIO::WriteQuat(std::ofstream& stream, const GobotQuat& quat)
    {
        if (!WriteFloat(stream, quat.m_x)) return false;
        if (!WriteFloat(stream, quat.m_y)) return false;
        if (!WriteFloat(stream, quat.m_z)) return false;
        if (!WriteFloat(stream, quat.m_w)) return false;
        return true;
    }

    bool GobotFileIO::WriteColor(std::ofstream& stream, const GobotColor& color)
    {
        if (!WriteFloat(stream, color.m_r)) return false;
        if (!WriteFloat(stream, color.m_g)) return false;
        if (!WriteFloat(stream, color.m_b)) return false;
        if (!WriteFloat(stream, color.m_a)) return false;
        return true;
    }

    bool GobotFileIO::WriteString(std::ofstream& stream, const std::string& str)
    {
        uint32_t len = static_cast<uint32_t>(str.size());
        if (!WriteUint32(stream, len)) return false;
        if (len > 0)
        {
            stream.write(str.data(), len);
        }
        return stream.good();
    }

    //=============================================================================
    // Read helpers
    //=============================================================================

    bool GobotFileIO::ReadUint8(std::ifstream& stream, uint8_t& value)
    {
        stream.read(reinterpret_cast<char*>(&value), sizeof(value));
        return stream.good();
    }

    bool GobotFileIO::ReadUint32(std::ifstream& stream, uint32_t& value)
    {
        stream.read(reinterpret_cast<char*>(&value), sizeof(value));
        return stream.good();
    }

    bool GobotFileIO::ReadFloat(std::ifstream& stream, float& value)
    {
        stream.read(reinterpret_cast<char*>(&value), sizeof(value));
        return stream.good();
    }

    bool GobotFileIO::ReadVec3(std::ifstream& stream, GobotVec3& vec)
    {
        if (!ReadFloat(stream, vec.m_x)) return false;
        if (!ReadFloat(stream, vec.m_y)) return false;
        if (!ReadFloat(stream, vec.m_z)) return false;
        return true;
    }

    bool GobotFileIO::ReadQuat(std::ifstream& stream, GobotQuat& quat)
    {
        if (!ReadFloat(stream, quat.m_x)) return false;
        if (!ReadFloat(stream, quat.m_y)) return false;
        if (!ReadFloat(stream, quat.m_z)) return false;
        if (!ReadFloat(stream, quat.m_w)) return false;
        return true;
    }

    bool GobotFileIO::ReadColor(std::ifstream& stream, GobotColor& color)
    {
        if (!ReadFloat(stream, color.m_r)) return false;
        if (!ReadFloat(stream, color.m_g)) return false;
        if (!ReadFloat(stream, color.m_b)) return false;
        if (!ReadFloat(stream, color.m_a)) return false;
        return true;
    }

    bool GobotFileIO::ReadString(std::ifstream& stream, std::string& str)
    {
        uint32_t len = 0;
        if (!ReadUint32(stream, len)) return false;
        str.resize(len);
        if (len > 0)
        {
            stream.read(&str[0], len);
        }
        return stream.good();
    }

    bool GobotFileIO::ValidateMagic(std::ifstream& stream, const char* expectedMagic, size_t magicLen)
    {
        char magic[8] = {0};
        stream.read(magic, magicLen);
        if (!stream.good())
        {
            return false;
        }
        return std::memcmp(magic, expectedMagic, magicLen) == 0;
    }

    //=============================================================================
    // Action File I/O
    //=============================================================================

    bool GobotFileIO::SaveAction(const GobotActionFile& action, const std::string& path)
    {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            s_lastStatus = GobotStatus::FileNotFound;
            return false;
        }

        // Write header
        file.write(action.m_header.m_magic, 8);
        if (!WriteUint32(file, action.m_header.m_version)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteUint32(file, action.m_header.m_boneCount)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteUint32(file, action.m_header.m_frameCount)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteFloat(file, action.m_header.m_duration)) { s_lastStatus = GobotStatus::WriteError; return false; }

        // Write bone names
        for (const auto& bone : action.m_bones)
        {
            if (!WriteString(file, bone.m_name)) { s_lastStatus = GobotStatus::WriteError; return false; }
        }

        // Write bone parents
        for (const auto& bone : action.m_bones)
        {
            if (!WriteUint32(file, static_cast<uint32_t>(bone.m_parentIndex))) { s_lastStatus = GobotStatus::WriteError; return false; }
        }

        // Write frame data
        for (const auto& frame : action.m_frames)
        {
            if (!WriteFloat(file, frame.m_time)) { s_lastStatus = GobotStatus::WriteError; return false; }
            if (!WriteUint32(file, frame.m_boneIndex)) { s_lastStatus = GobotStatus::WriteError; return false; }
            if (!WriteVec3(file, frame.m_position)) { s_lastStatus = GobotStatus::WriteError; return false; }
            if (!WriteQuat(file, frame.m_rotation)) { s_lastStatus = GobotStatus::WriteError; return false; }
        }

        s_lastStatus = GobotStatus::Success;
        return true;
    }

    bool GobotFileIO::LoadAction(const std::string& path, GobotActionFile& action)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            s_lastStatus = GobotStatus::FileNotFound;
            return false;
        }

        // Validate magic
        if (!ValidateMagic(file, GOBOT_ACTION_MAGIC, 8))
        {
            s_lastStatus = GobotStatus::InvalidMagic;
            return false;
        }

        // Read header
        if (!ReadUint32(file, action.m_header.m_version)) { s_lastStatus = GobotStatus::ReadError; return false; }
        if (action.m_header.m_version > GOBOT_ACTION_VERSION)
        {
            s_lastStatus = GobotStatus::UnsupportedVersion;
            return false;
        }

        if (!ReadUint32(file, action.m_header.m_boneCount)) { s_lastStatus = GobotStatus::ReadError; return false; }
        if (!ReadUint32(file, action.m_header.m_frameCount)) { s_lastStatus = GobotStatus::ReadError; return false; }
        if (!ReadFloat(file, action.m_header.m_duration)) { s_lastStatus = GobotStatus::ReadError; return false; }

        // Read bone names
        action.m_bones.resize(action.m_header.m_boneCount);
        for (uint32_t i = 0; i < action.m_header.m_boneCount; i++)
        {
            if (!ReadString(file, action.m_bones[i].m_name)) { s_lastStatus = GobotStatus::ReadError; return false; }
        }

        // Read bone parents
        for (uint32_t i = 0; i < action.m_header.m_boneCount; i++)
        {
            uint32_t parentIdx = 0;
            if (!ReadUint32(file, parentIdx)) { s_lastStatus = GobotStatus::ReadError; return false; }
            action.m_bones[i].m_parentIndex = static_cast<int32_t>(parentIdx);
        }

        // Read frame data
        action.m_frames.resize(action.m_header.m_frameCount);
        for (uint32_t i = 0; i < action.m_header.m_frameCount; i++)
        {
            if (!ReadFloat(file, action.m_frames[i].m_time)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadUint32(file, action.m_frames[i].m_boneIndex)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadVec3(file, action.m_frames[i].m_position)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadQuat(file, action.m_frames[i].m_rotation)) { s_lastStatus = GobotStatus::ReadError; return false; }
        }

        s_lastStatus = GobotStatus::Success;
        return true;
    }

    //=============================================================================
    // Scene File I/O
    //=============================================================================

    bool GobotFileIO::SaveScene(const GobotSceneFile& scene, const std::string& path)
    {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            s_lastStatus = GobotStatus::FileNotFound;
            return false;
        }

        // Write header
        file.write(scene.m_header.m_magic, 8);
        if (!WriteUint32(file, scene.m_header.m_version)) { s_lastStatus = GobotStatus::WriteError; return false; }

        // Write camera
        if (!WriteVec3(file, scene.m_camera.m_position)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteVec3(file, scene.m_camera.m_target)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteFloat(file, scene.m_camera.m_fov)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteFloat(file, scene.m_camera.m_nearClip)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteFloat(file, scene.m_camera.m_farClip)) { s_lastStatus = GobotStatus::WriteError; return false; }

        // Write lights
        uint32_t lightCount = static_cast<uint32_t>(scene.m_lights.size());
        if (!WriteUint32(file, lightCount)) { s_lastStatus = GobotStatus::WriteError; return false; }
        for (const auto& light : scene.m_lights)
        {
            if (!WriteUint32(file, static_cast<uint32_t>(light.m_type))) { s_lastStatus = GobotStatus::WriteError; return false; }
            if (!WriteVec3(file, light.m_position)) { s_lastStatus = GobotStatus::WriteError; return false; }
            if (!WriteVec3(file, light.m_direction)) { s_lastStatus = GobotStatus::WriteError; return false; }
            if (!WriteColor(file, light.m_color)) { s_lastStatus = GobotStatus::WriteError; return false; }
            if (!WriteFloat(file, light.m_intensity)) { s_lastStatus = GobotStatus::WriteError; return false; }
        }

        // Write environment
        if (!WriteUint32(file, static_cast<uint32_t>(scene.m_environment.m_groundType))) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteColor(file, scene.m_environment.m_ambientColor)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteColor(file, scene.m_environment.m_fogColor)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteFloat(file, scene.m_environment.m_fogDensity)) { s_lastStatus = GobotStatus::WriteError; return false; }

        // Write primitives
        uint32_t primCount = static_cast<uint32_t>(scene.m_primitives.size());
        if (!WriteUint32(file, primCount)) { s_lastStatus = GobotStatus::WriteError; return false; }
        for (const auto& prim : scene.m_primitives)
        {
            if (!WriteUint32(file, static_cast<uint32_t>(prim.m_type))) { s_lastStatus = GobotStatus::WriteError; return false; }
            if (!WriteVec3(file, prim.m_size)) { s_lastStatus = GobotStatus::WriteError; return false; }
            if (!WriteVec3(file, prim.m_position)) { s_lastStatus = GobotStatus::WriteError; return false; }
            if (!WriteVec3(file, prim.m_rotation)) { s_lastStatus = GobotStatus::WriteError; return false; }
            if (!WriteColor(file, prim.m_color)) { s_lastStatus = GobotStatus::WriteError; return false; }
        }

        // Write filter
        if (!WriteUint32(file, static_cast<uint32_t>(scene.m_filter.m_filterType))) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteFloat(file, scene.m_filter.m_exposure)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteFloat(file, scene.m_filter.m_contrast)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteFloat(file, scene.m_filter.m_saturation)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteFloat(file, scene.m_filter.m_hueShift)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteFloat(file, scene.m_filter.m_temperature)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteFloat(file, scene.m_filter.m_tint)) { s_lastStatus = GobotStatus::WriteError; return false; }

        s_lastStatus = GobotStatus::Success;
        return true;
    }

    bool GobotFileIO::LoadScene(const std::string& path, GobotSceneFile& scene)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            s_lastStatus = GobotStatus::FileNotFound;
            return false;
        }

        // Validate magic
        if (!ValidateMagic(file, GOBOT_SCENE_MAGIC, 8))
        {
            s_lastStatus = GobotStatus::InvalidMagic;
            return false;
        }

        // Read header
        if (!ReadUint32(file, scene.m_header.m_version)) { s_lastStatus = GobotStatus::ReadError; return false; }
        if (scene.m_header.m_version > GOBOT_SCENE_VERSION)
        {
            s_lastStatus = GobotStatus::UnsupportedVersion;
            return false;
        }

        // Read camera
        if (!ReadVec3(file, scene.m_camera.m_position)) { s_lastStatus = GobotStatus::ReadError; return false; }
        if (!ReadVec3(file, scene.m_camera.m_target)) { s_lastStatus = GobotStatus::ReadError; return false; }
        if (!ReadFloat(file, scene.m_camera.m_fov)) { s_lastStatus = GobotStatus::ReadError; return false; }
        if (!ReadFloat(file, scene.m_camera.m_nearClip)) { s_lastStatus = GobotStatus::ReadError; return false; }
        if (!ReadFloat(file, scene.m_camera.m_farClip)) { s_lastStatus = GobotStatus::ReadError; return false; }

        // Read lights
        uint32_t lightCount = 0;
        if (!ReadUint32(file, lightCount)) { s_lastStatus = GobotStatus::ReadError; return false; }
        scene.m_lights.resize(lightCount);
        for (uint32_t i = 0; i < lightCount; i++)
        {
            uint32_t typeVal = 0;
            if (!ReadUint32(file, typeVal)) { s_lastStatus = GobotStatus::ReadError; return false; }
            scene.m_lights[i].m_type = static_cast<GobotLightType>(typeVal);
            if (!ReadVec3(file, scene.m_lights[i].m_position)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadVec3(file, scene.m_lights[i].m_direction)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadColor(file, scene.m_lights[i].m_color)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadFloat(file, scene.m_lights[i].m_intensity)) { s_lastStatus = GobotStatus::ReadError; return false; }
        }

        // Read environment
        {
            uint32_t groundType = 0;
            if (!ReadUint32(file, groundType)) { s_lastStatus = GobotStatus::ReadError; return false; }
            scene.m_environment.m_groundType = static_cast<GobotGroundType>(groundType);
            if (!ReadColor(file, scene.m_environment.m_ambientColor)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadColor(file, scene.m_environment.m_fogColor)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadFloat(file, scene.m_environment.m_fogDensity)) { s_lastStatus = GobotStatus::ReadError; return false; }
        }

        // Read primitives
        uint32_t primCount = 0;
        if (!ReadUint32(file, primCount)) { s_lastStatus = GobotStatus::ReadError; return false; }
        scene.m_primitives.resize(primCount);
        for (uint32_t i = 0; i < primCount; i++)
        {
            uint32_t typeVal = 0;
            if (!ReadUint32(file, typeVal)) { s_lastStatus = GobotStatus::ReadError; return false; }
            scene.m_primitives[i].m_type = static_cast<GobotPrimitiveType>(typeVal);
            if (!ReadVec3(file, scene.m_primitives[i].m_size)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadVec3(file, scene.m_primitives[i].m_position)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadVec3(file, scene.m_primitives[i].m_rotation)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadColor(file, scene.m_primitives[i].m_color)) { s_lastStatus = GobotStatus::ReadError; return false; }
        }

        // Read filter
        {
            uint32_t filterType = 0;
            if (!ReadUint32(file, filterType)) { s_lastStatus = GobotStatus::ReadError; return false; }
            scene.m_filter.m_filterType = static_cast<GobotFilterType>(filterType);
            if (!ReadFloat(file, scene.m_filter.m_exposure)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadFloat(file, scene.m_filter.m_contrast)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadFloat(file, scene.m_filter.m_saturation)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadFloat(file, scene.m_filter.m_hueShift)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadFloat(file, scene.m_filter.m_temperature)) { s_lastStatus = GobotStatus::ReadError; return false; }
            if (!ReadFloat(file, scene.m_filter.m_tint)) { s_lastStatus = GobotStatus::ReadError; return false; }
        }

        s_lastStatus = GobotStatus::Success;
        return true;
    }

    //=============================================================================
    // Project File I/O
    //=============================================================================

    bool GobotFileIO::SaveProject(const GobotProjectFile& project, const std::string& path)
    {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            s_lastStatus = GobotStatus::FileNotFound;
            return false;
        }

        // Write header
        file.write(project.m_header.m_magic, 8);
        if (!WriteUint32(file, project.m_header.m_version)) { s_lastStatus = GobotStatus::WriteError; return false; }

        // Write model paths
        uint32_t modelCount = static_cast<uint32_t>(project.m_modelPaths.size());
        if (!WriteUint32(file, modelCount)) { s_lastStatus = GobotStatus::WriteError; return false; }
        for (const auto& modelPath : project.m_modelPaths)
        {
            if (!WriteString(file, modelPath)) { s_lastStatus = GobotStatus::WriteError; return false; }
        }

        // Write action paths
        uint32_t actionCount = static_cast<uint32_t>(project.m_actionPaths.size());
        if (!WriteUint32(file, actionCount)) { s_lastStatus = GobotStatus::WriteError; return false; }
        for (const auto& actionPath : project.m_actionPaths)
        {
            if (!WriteString(file, actionPath)) { s_lastStatus = GobotStatus::WriteError; return false; }
        }

        // Write embedded scene
        // Re-use the scene serialization by writing scene data inline
        // Scene header (already embedded in GobotSceneFile)
        if (!SaveScene(project.m_scene, path + ".tmp_scene"))
        {
            s_lastStatus = GobotStatus::WriteError;
            return false;
        }
        // Read the temp scene file and append it
        {
            std::ifstream tmpFile(path + ".tmp_scene", std::ios::binary);
            if (!tmpFile.is_open())
            {
                s_lastStatus = GobotStatus::WriteError;
                return false;
            }
            file << tmpFile.rdbuf();
            if (!file.good())
            {
                s_lastStatus = GobotStatus::WriteError;
                return false;
            }
        }
        // Remove temp file
        std::remove((path + ".tmp_scene").c_str());

        // Write UI layout
        file.put(project.m_hasUILayout ? 1 : 0);
        if (project.m_hasUILayout)
        {
            uint32_t windowCount = static_cast<uint32_t>(project.m_uiWindows.size());
            if (!WriteUint32(file, windowCount)) { s_lastStatus = GobotStatus::WriteError; return false; }
            for (const auto& wnd : project.m_uiWindows)
            {
                if (!WriteString(file, wnd.m_name)) { s_lastStatus = GobotStatus::WriteError; return false; }
                if (!WriteFloat(file, wnd.m_x)) { s_lastStatus = GobotStatus::WriteError; return false; }
                if (!WriteFloat(file, wnd.m_y)) { s_lastStatus = GobotStatus::WriteError; return false; }
                if (!WriteFloat(file, wnd.m_width)) { s_lastStatus = GobotStatus::WriteError; return false; }
                if (!WriteFloat(file, wnd.m_height)) { s_lastStatus = GobotStatus::WriteError; return false; }
            }
        }

        // Write settings
        if (!WriteString(file, project.m_language)) { s_lastStatus = GobotStatus::WriteError; return false; }
        if (!WriteUint32(file, static_cast<uint32_t>(project.m_orientation))) { s_lastStatus = GobotStatus::WriteError; return false; }

        s_lastStatus = GobotStatus::Success;
        return true;
    }

    bool GobotFileIO::LoadProject(const std::string& path, GobotProjectFile& project)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            s_lastStatus = GobotStatus::FileNotFound;
            return false;
        }

        // Validate magic
        if (!ValidateMagic(file, GOBOT_PROJECT_MAGIC, 8))
        {
            s_lastStatus = GobotStatus::InvalidMagic;
            return false;
        }

        // Read header
        if (!ReadUint32(file, project.m_header.m_version)) { s_lastStatus = GobotStatus::ReadError; return false; }
        if (project.m_header.m_version > GOBOT_PROJECT_VERSION)
        {
            s_lastStatus = GobotStatus::UnsupportedVersion;
            return false;
        }

        // Read model paths
        uint32_t modelCount = 0;
        if (!ReadUint32(file, modelCount)) { s_lastStatus = GobotStatus::ReadError; return false; }
        project.m_modelPaths.resize(modelCount);
        for (uint32_t i = 0; i < modelCount; i++)
        {
            if (!ReadString(file, project.m_modelPaths[i])) { s_lastStatus = GobotStatus::ReadError; return false; }
        }

        // Read action paths
        uint32_t actionCount = 0;
        if (!ReadUint32(file, actionCount)) { s_lastStatus = GobotStatus::ReadError; return false; }
        project.m_actionPaths.resize(actionCount);
        for (uint32_t i = 0; i < actionCount; i++)
        {
            if (!ReadString(file, project.m_actionPaths[i])) { s_lastStatus = GobotStatus::ReadError; return false; }
        }

        // Read embedded scene
        if (!LoadScene(path, project.m_scene))
        {
            // If scene loading fails, it might be because we're reading from within the project file
            // Seek back to after the action paths and read scene data manually
            // This is a simplified approach - in a full implementation we'd track offsets
            s_lastStatus = GobotStatus::ReadError;
            return false;
        }

        // Read UI layout
        {
            char hasUI = 0;
            file.get(hasUI);
            if (!file.good())
            {
                s_lastStatus = GobotStatus::ReadError;
                return false;
            }
            project.m_hasUILayout = (hasUI != 0);
            if (project.m_hasUILayout)
            {
                uint32_t windowCount = 0;
                if (!ReadUint32(file, windowCount)) { s_lastStatus = GobotStatus::ReadError; return false; }
                project.m_uiWindows.resize(windowCount);
                for (uint32_t i = 0; i < windowCount; i++)
                {
                    if (!ReadString(file, project.m_uiWindows[i].m_name)) { s_lastStatus = GobotStatus::ReadError; return false; }
                    if (!ReadFloat(file, project.m_uiWindows[i].m_x)) { s_lastStatus = GobotStatus::ReadError; return false; }
                    if (!ReadFloat(file, project.m_uiWindows[i].m_y)) { s_lastStatus = GobotStatus::ReadError; return false; }
                    if (!ReadFloat(file, project.m_uiWindows[i].m_width)) { s_lastStatus = GobotStatus::ReadError; return false; }
                    if (!ReadFloat(file, project.m_uiWindows[i].m_height)) { s_lastStatus = GobotStatus::ReadError; return false; }
                }
            }
        }

        // Read settings
        if (!ReadString(file, project.m_language)) { s_lastStatus = GobotStatus::ReadError; return false; }
        {
            uint32_t orientationVal = 0;
            if (!ReadUint32(file, orientationVal)) { s_lastStatus = GobotStatus::ReadError; return false; }
            project.m_orientation = static_cast<GobotOrientation>(orientationVal);
        }

        s_lastStatus = GobotStatus::Success;
        return true;
    }

} // namespace gobot