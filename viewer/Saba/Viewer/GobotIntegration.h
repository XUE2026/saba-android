//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_GOBOTINTEGRATION_H_
#define SABA_VIEWER_GOBOTINTEGRATION_H_

#include <gobot/GobotTypes.h>
#include <gobot/GobotFileIO.h>
#include <gobot/GobotConverter.h>

#include <string>

namespace saba
{
    class Viewer;

    class GobotIntegration
    {
    public:
        explicit GobotIntegration(Viewer* viewer);
        ~GobotIntegration();

        GobotIntegration(const GobotIntegration&) = delete;
        GobotIntegration& operator=(const GobotIntegration&) = delete;

        // Load and apply a .gobot_action file to the current model
        bool LoadGobotAction(const std::string& path);

        // Load and apply a .gobot_scene file
        bool LoadGobotScene(const std::string& path);

        // Load an entire .gobot project
        bool LoadGobotProject(const std::string& path);

        // Save current animation as .gobot_action
        bool SaveCurrentAsGobotAction(const std::string& path);

        // Save current scene as .gobot_scene
        bool SaveCurrentAsGobotScene(const std::string& path);

        // Get last operation status
        gobot::GobotStatus GetLastStatus() const;

    private:
        bool ApplyActionToViewer(const gobot::GobotActionFile& action);
        bool ApplySceneToViewer(const gobot::GobotSceneFile& scene);
        bool CaptureViewerAction(gobot::GobotActionFile& action);
        bool CaptureViewerScene(gobot::GobotSceneFile& scene);

        Viewer* m_viewer;
    };

} // namespace saba

#endif // !SABA_VIEWER_GOBOTINTEGRATION_H_