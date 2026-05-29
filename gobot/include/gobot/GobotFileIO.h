//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef GOBOT_GOBOTFILEIO_H_
#define GOBOT_GOBOTFILEIO_H_

#include "GobotTypes.h"

#include <string>

namespace gobot
{
    class GobotFileIO
    {
    public:
        // Action file I/O
        static bool SaveAction(const GobotActionFile& action, const std::string& path);
        static bool LoadAction(const std::string& path, GobotActionFile& action);

        // Scene file I/O
        static bool SaveScene(const GobotSceneFile& scene, const std::string& path);
        static bool LoadScene(const std::string& path, GobotSceneFile& scene);

        // Project file I/O
        static bool SaveProject(const GobotProjectFile& project, const std::string& path);
        static bool LoadProject(const std::string& path, GobotProjectFile& project);

        // Utility
        static GobotStatus GetLastStatus() { return s_lastStatus; }

    private:
        // Internal write helpers
        static bool WriteUint8(std::ofstream& stream, uint8_t value);
        static bool WriteUint32(std::ofstream& stream, uint32_t value);
        static bool WriteFloat(std::ofstream& stream, float value);
        static bool WriteVec3(std::ofstream& stream, const GobotVec3& vec);
        static bool WriteQuat(std::ofstream& stream, const GobotQuat& quat);
        static bool WriteColor(std::ofstream& stream, const GobotColor& color);
        static bool WriteString(std::ofstream& stream, const std::string& str);

        // Internal read helpers
        static bool ReadUint8(std::ifstream& stream, uint8_t& value);
        static bool ReadUint32(std::ifstream& stream, uint32_t& value);
        static bool ReadFloat(std::ifstream& stream, float& value);
        static bool ReadVec3(std::ifstream& stream, GobotVec3& vec);
        static bool ReadQuat(std::ifstream& stream, GobotQuat& quat);
        static bool ReadColor(std::ifstream& stream, GobotColor& color);
        static bool ReadString(std::ifstream& stream, std::string& str);

        // Magic validation
        static bool ValidateMagic(std::ifstream& stream, const char* expectedMagic, size_t magicLen);

        static GobotStatus s_lastStatus;
    };

} // namespace gobot

#endif // !GOBOT_GOBOTFILEIO_H_