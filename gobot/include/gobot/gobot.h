//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef GOBOT_GOBOT_H_
#define GOBOT_GOBOT_H_

#include <cstdint>
#include <string>

namespace gobot
{
    constexpr const char* GOBOT_VERSION_STRING = "1.0.0";
    constexpr uint32_t    GOBOT_VERSION_MAJOR = 1;
    constexpr uint32_t    GOBOT_VERSION_MINOR = 0;
    constexpr uint32_t    GOBOT_VERSION_PATCH = 0;

    // File format magic numbers
    constexpr const char* GOBOT_ACTION_MAGIC = "GOBOT_ACT";
    constexpr const char* GOBOT_SCENE_MAGIC  = "GOBOT_SCN";
    constexpr const char* GOBOT_PROJECT_MAGIC = "GOBOT_PRJ";

    // Current file format versions
    constexpr uint32_t GOBOT_ACTION_VERSION  = 1;
    constexpr uint32_t GOBOT_SCENE_VERSION   = 1;
    constexpr uint32_t GOBOT_PROJECT_VERSION = 1;

    // File extensions
    constexpr const char* GOBOT_ACTION_EXT  = ".gobot_action";
    constexpr const char* GOBOT_SCENE_EXT   = ".gobot_scene";
    constexpr const char* GOBOT_PROJECT_EXT = ".gobot_proj";

    // Status codes for file operations
    enum class GobotStatus
    {
        Success,
        FileNotFound,
        InvalidMagic,
        UnsupportedVersion,
        ReadError,
        WriteError,
        InvalidData,
        ConversionError,
        NotImplemented
    };

    inline const char* GobotStatusToString(GobotStatus status)
    {
        switch (status)
        {
            case GobotStatus::Success:           return "Success";
            case GobotStatus::FileNotFound:      return "File not found";
            case GobotStatus::InvalidMagic:      return "Invalid magic number";
            case GobotStatus::UnsupportedVersion: return "Unsupported version";
            case GobotStatus::ReadError:         return "Read error";
            case GobotStatus::WriteError:        return "Write error";
            case GobotStatus::InvalidData:       return "Invalid data";
            case GobotStatus::ConversionError:   return "Conversion error";
            case GobotStatus::NotImplemented:    return "Not implemented";
            default:                             return "Unknown status";
        }
    }

    // Light type enum
    enum class GobotLightType : uint32_t
    {
        Directional = 0,
        Point       = 1,
        Spot        = 2
    };

    // Ground type enum
    enum class GobotGroundType : uint32_t
    {
        Floor   = 0,
        Grass   = 1,
        Mud     = 2,
        Asphalt = 3,
        Sky     = 4,
        Field   = 5
    };

    // Primitive type enum
    enum class GobotPrimitiveType : uint32_t
    {
        Cube     = 0,
        Sphere   = 1,
        Cone     = 2,
        Cylinder = 3
    };

    // Filter type enum
    enum class GobotFilterType : uint32_t
    {
        None        = 0,
        BrightWhite = 1,
        SkinBright  = 2,
        Dark        = 3,
        GenshinDay  = 4,
        GenshinNight = 5,
        GenshinDusk = 6,
        Custom      = 7
    };

    // Orientation enum
    enum class GobotOrientation : uint32_t
    {
        Portrait  = 0,
        Landscape = 1
    };

} // namespace gobot

#endif // !GOBOT_GOBOT_H_