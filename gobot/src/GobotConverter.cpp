//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include <gobot/GobotConverter.h>
#include <gobot/GobotFileIO.h>

#include <cstring>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

namespace gobot
{
    GobotStatus GobotConverter::s_lastStatus = GobotStatus::Success;

    //=============================================================================
    // VMD to GoBotAction conversion
    //=============================================================================

    bool GobotConverter::ImportVMDToAction(const std::string& vmdPath, GobotActionFile& action)
    {
        // Open VMD file
        std::ifstream file(vmdPath, std::ios::binary);
        if (!file.is_open())
        {
            s_lastStatus = GobotStatus::FileNotFound;
            return false;
        }

        // Read VMD header (30 bytes header + 20 bytes model name)
        // VMD format: 30-byte header string, 20-byte model name
        char header[30];
        char modelName[20];
        file.read(header, 30);
        file.read(modelName, 20);
        if (!file.good())
        {
            s_lastStatus = GobotStatus::ReadError;
            return false;
        }

        // Validate VMD magic
        if (std::memcmp(header, "Vocaloid Motion Data", 19) != 0 &&
            std::memcmp(header, "Vocaloid Motion Data 2", 22) != 0)
        {
            s_lastStatus = GobotStatus::InvalidMagic;
            return false;
        }

        // Read motion count
        uint32_t motionCount = 0;
        file.read(reinterpret_cast<char*>(&motionCount), sizeof(motionCount));
        if (!file.good())
        {
            s_lastStatus = GobotStatus::ReadError;
            return false;
        }

        // Set action header
        action.m_header.m_frameCount = motionCount;
        action.m_header.m_duration = 0.0f;

        // Temporary storage for bone name deduplication
        std::vector<std::string> boneNames;
        std::vector<int32_t> boneParents;

        // Read motion data
        action.m_frames.resize(motionCount);
        for (uint32_t i = 0; i < motionCount; i++)
        {
            // VMDMotion: 15-byte bone name, uint32 frame, vec3 translate, quat rotate, 64-byte interpolation
            char boneName[15] = {0};
            file.read(boneName, 15);

            uint32_t frame = 0;
            file.read(reinterpret_cast<char*>(&frame), sizeof(frame));

            float pos[3];
            file.read(reinterpret_cast<char*>(pos), sizeof(pos));

            float quat[4];
            file.read(reinterpret_cast<char*>(quat), sizeof(quat));

            // Skip interpolation data (64 bytes)
            file.seekg(64, std::ios::cur);

            if (!file.good())
            {
                s_lastStatus = GobotStatus::ReadError;
                return false;
            }

            // Find or create bone index
            std::string boneNameStr(boneName);
            auto it = std::find(boneNames.begin(), boneNames.end(), boneNameStr);
            uint32_t boneIndex = 0;
            if (it == boneNames.end())
            {
                boneIndex = static_cast<uint32_t>(boneNames.size());
                boneNames.push_back(boneNameStr);
                boneParents.push_back(-1);
            }
            else
            {
                boneIndex = static_cast<uint32_t>(std::distance(boneNames.begin(), it));
            }

            // Fill frame data
            action.m_frames[i].m_time = static_cast<float>(frame);
            action.m_frames[i].m_boneIndex = boneIndex;
            action.m_frames[i].m_position = GobotVec3(pos[0], pos[1], pos[2]);
            action.m_frames[i].m_rotation = GobotQuat(quat[0], quat[1], quat[2], quat[3]);

            // Track max time for duration
            if (action.m_frames[i].m_time > action.m_header.m_duration)
            {
                action.m_header.m_duration = action.m_frames[i].m_time;
            }
        }

        // Fill bone info
        action.m_header.m_boneCount = static_cast<uint32_t>(boneNames.size());
        action.m_bones.resize(boneNames.size());
        for (size_t i = 0; i < boneNames.size(); i++)
        {
            action.m_bones[i].m_name = boneNames[i];
            action.m_bones[i].m_parentIndex = boneParents[i];
        }

        s_lastStatus = GobotStatus::Success;
        return true;
    }

    //=============================================================================
    // GoBotAction to VMD conversion
    //=============================================================================

    bool GobotConverter::ExportActionToVMD(const GobotActionFile& action, const std::string& vmdPath)
    {
        std::ofstream file(vmdPath, std::ios::binary);
        if (!file.is_open())
        {
            s_lastStatus = GobotStatus::FileNotFound;
            return false;
        }

        // Write VMD header
        char header[30];
        std::memset(header, 0, sizeof(header));
        const char* vmdHeaderStr = "Vocaloid Motion Data 2";
        std::memcpy(header, vmdHeaderStr, std::min<size_t>(strlen(vmdHeaderStr), sizeof(header) - 1));
        file.write(header, 30);

        // Write model name (empty for generic)
        char modelName[20] = {0};
        file.write(modelName, 20);

        // Write motion count
        uint32_t motionCount = action.m_header.m_frameCount;
        file.write(reinterpret_cast<const char*>(&motionCount), sizeof(motionCount));

        // Write motion data
        for (const auto& frame : action.m_frames)
        {
            // Write bone name (15 bytes, padded)
            char boneName[15] = {0};
            if (frame.m_boneIndex < action.m_bones.size())
            {
                const std::string& name = action.m_bones[frame.m_boneIndex].m_name;
                size_t copyLen = std::min<size_t>(name.size(), 14);
                std::memcpy(boneName, name.c_str(), copyLen);
            }
            file.write(boneName, 15);

            // Write frame number
            uint32_t frameNum = static_cast<uint32_t>(frame.m_time);
            file.write(reinterpret_cast<const char*>(&frameNum), sizeof(frameNum));

            // Write position
            float pos[3] = { frame.m_position.m_x, frame.m_position.m_y, frame.m_position.m_z };
            file.write(reinterpret_cast<const char*>(pos), sizeof(pos));

            // Write rotation
            float quat[4] = { frame.m_rotation.m_x, frame.m_rotation.m_y, frame.m_rotation.m_z, frame.m_rotation.m_w };
            file.write(reinterpret_cast<const char*>(quat), sizeof(quat));

            // Write default interpolation (64 bytes)
            uint8_t defaultInterp[64] = {
                0, 0, 0, 0,  // X: linear
                0, 0, 0, 0,  // Y: linear
                0, 0, 0, 0,  // Z: linear
                0, 0, 0, 0,  // Rotation: linear
                // Remaining bytes set to 0
            };
            file.write(reinterpret_cast<const char*>(defaultInterp), 64);

            if (!file.good())
            {
                s_lastStatus = GobotStatus::WriteError;
                return false;
            }
        }

        s_lastStatus = GobotStatus::Success;
        return true;
    }

    //=============================================================================
    // Saba Scene Import/Export (stub implementations)
    //=============================================================================

    bool GobotConverter::ImportSabaScene(GobotSceneFile& scene)
    {
        // This function would query the current Saba viewer state
        // and populate the GobotSceneFile structure.
        // In a real implementation, this would access ViewerContext,
        // Camera, Light, EnvironmentManager, and FilterPanel instances.

        // For now, populate with default values
        // The caller is expected to fill in the details
        scene = GobotSceneFile();

        s_lastStatus = GobotStatus::NotImplemented;
        return false;
    }

    bool GobotConverter::ExportToSabaScene(const GobotSceneFile& scene)
    {
        // This function would apply the GobotSceneFile data
        // to the current Saba viewer state.
        // In a real implementation, this would set Camera, Light,
        // EnvironmentManager, and FilterPanel from the scene data.

        s_lastStatus = GobotStatus::NotImplemented;
        return false;
    }

} // namespace gobot