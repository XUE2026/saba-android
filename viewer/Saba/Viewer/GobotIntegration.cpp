//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "GobotIntegration.h"
#include "Viewer.h"
#include "ViewerContext.h"
#include "EnvironmentManager.h"
#include "FilterPanel.h"

#include <Saba/GL/GLFilter.h>

#include <gobot/gobot.h>
#include <gobot/GobotFileIO.h>

namespace saba
{
    GobotIntegration::GobotIntegration(Viewer* viewer)
        : m_viewer(viewer)
    {
    }

    GobotIntegration::~GobotIntegration()
    {
    }

    gobot::GobotStatus GobotIntegration::GetLastStatus() const
    {
        return gobot::GobotFileIO::GetLastStatus();
    }

    bool GobotIntegration::LoadGobotAction(const std::string& path)
    {
        gobot::GobotActionFile action;
        if (!gobot::GobotFileIO::LoadAction(path, action))
        {
            return false;
        }
        return ApplyActionToViewer(action);
    }

    bool GobotIntegration::LoadGobotScene(const std::string& path)
    {
        gobot::GobotSceneFile scene;
        if (!gobot::GobotFileIO::LoadScene(path, scene))
        {
            return false;
        }
        return ApplySceneToViewer(scene);
    }

    bool GobotIntegration::LoadGobotProject(const std::string& path)
    {
        gobot::GobotProjectFile project;
        if (!gobot::GobotFileIO::LoadProject(path, project))
        {
            return false;
        }

        // Load scene settings
        if (!ApplySceneToViewer(project.m_scene))
        {
            return false;
        }

        // Note: Model loading and action loading requires additional
        // viewer API access. The project file records the paths but
        // the actual loading depends on the current viewer state.

        return true;
    }

    bool GobotIntegration::SaveCurrentAsGobotAction(const std::string& path)
    {
        gobot::GobotActionFile action;
        if (!CaptureViewerAction(action))
        {
            return false;
        }
        return gobot::GobotFileIO::SaveAction(action, path);
    }

    bool GobotIntegration::SaveCurrentAsGobotScene(const std::string& path)
    {
        gobot::GobotSceneFile scene;
        if (!CaptureViewerScene(scene))
        {
            return false;
        }
        return gobot::GobotFileIO::SaveScene(scene, path);
    }

    bool GobotIntegration::ApplyActionToViewer(const gobot::GobotActionFile& action)
    {
        // This would apply bone keyframe data to the currently selected model.
        // In a full implementation, this would iterate over the action frames
        // and set bone transforms on the MMD model's animation system.
        //
        // For now, this returns true as a placeholder since the full
        // animation system integration requires VMDAnimation access.
        (void)action;
        return true;
    }

    bool GobotIntegration::ApplySceneToViewer(const gobot::GobotSceneFile& scene)
    {
        // This would apply camera, light, environment, and filter settings
        // to the current viewer state.
        //
        // In a full implementation:
        // 1. Set camera position/target from scene.m_camera
        // 2. Set light color/direction from scene.m_lights
        // 3. Set environment settings from scene.m_environment
        // 4. Set filter parameters from scene.m_filter
        // 5. Add primitives from scene.m_primitives
        (void)scene;
        return true;
    }

    bool GobotIntegration::CaptureViewerAction(gobot::GobotActionFile& action)
    {
        // This would capture the current animation state from the viewer
        // and populate the GobotActionFile structure.
        //
        // In a full implementation, this would iterate over all bone
        // controllers in the current MMD model and extract keyframe data.
        (void)action;
        return true;
    }

    bool GobotIntegration::CaptureViewerScene(gobot::GobotSceneFile& scene)
    {
        // This would capture the current scene state from the viewer
        // and populate the GobotSceneFile structure.
        //
        // In a full implementation, this would:
        // 1. Get camera position/target from viewer context
        // 2. Get light settings from viewer context
        // 3. Get environment settings from EnvironmentManager
        // 4. Get filter settings from FilterPanel/GLFilter
        // 5. Get primitive list from EnvironmentManager
        (void)scene;
        return true;
    }

} // namespace saba