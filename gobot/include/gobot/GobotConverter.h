//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef GOBOT_GOBOTCONVERTER_H_
#define GOBOT_GOBOTCONVERTER_H_

#include "GobotTypes.h"

#include <string>

namespace gobot
{
    class GobotConverter
    {
    public:
        // VMD to GoBotAction conversion
        static bool ImportVMDToAction(const std::string& vmdPath, GobotActionFile& action);

        // GoBotAction to VMD conversion
        static bool ExportActionToVMD(const GobotActionFile& action, const std::string& vmdPath);

        // Import current Saba viewer state to scene
        static bool ImportSabaScene(GobotSceneFile& scene);

        // Export scene to Saba viewer
        static bool ExportToSabaScene(const GobotSceneFile& scene);

        // Utility
        static GobotStatus GetLastStatus() { return s_lastStatus; }

    private:
        static GobotStatus s_lastStatus;
    };

} // namespace gobot

#endif // !GOBOT_GOBOTCONVERTER_H_