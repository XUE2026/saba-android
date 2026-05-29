# GoBot Framework for Saba MMD Viewer

## Overview

GoBot is a custom framework that defines file formats for actions, scenes, and projects for 3D model viewing and editing in the Saba MMD Viewer. It provides a standardized way to save and load animation data, scene configurations, and complete project setups.

## Version

Current version: 1.0.0

## File Format Specifications

### 1. Action Files (.gobot_action)

Binary format for skeletal animation keyframe data.

| Section | Description |
|---------|-------------|
| Header | Magic "GOBOT_ACT", version, bone/frame counts, duration |
| Bone Names | List of bone names as UTF-8 strings |
| Bone Parents | Parent index for each bone (-1 for root) |
| Frame Data | Array of keyframes with time, bone index, position, rotation |

### 2. Scene Files (.gobot_scene)

Binary format for complete scene configuration.

| Section | Description |
|---------|-------------|
| Header | Magic "GOBOT_SCN", version |
| Camera | Position, target, FOV, near/far clip |
| Lights | Array of light definitions (type, position, direction, color, intensity) |
| Environment | Ground type, ambient color, fog settings |
| Primitives | Array of primitive objects (type, size, position, rotation, color) |
| Filter | Filter type and parameters (exposure, contrast, saturation, etc.) |

### 3. Project Files (.gobot_proj)

Binary format for complete project configurations.

| Section | Description |
|---------|-------------|
| Header | Magic "GOBOT_PRJ", version |
| Model References | Paths to PMX/FBX model files |
| Action References | Paths to .gobot_action or VMD files |
| Scene Settings | Embedded GobotSceneFile data |
| UI Layout | Window positions and sizes (optional) |
| Settings | Language, orientation preference |

## Integration Guide

### Building

```bash
mkdir -p gobot/build && cd gobot/build
cmake ..
make
```

### Linking with Saba Viewer

Add to your CMakeLists.txt:

```cmake
add_subdirectory(gobot)
target_link_libraries(SabaViewer gobot)
target_include_directories(SabaViewer PRIVATE gobot/include)
```

### Basic Usage

```cpp
#include <gobot/gobot.h>
#include <gobot/GobotTypes.h>
#include <gobot/GobotFileIO.h>

// Save an action
gobot::GobotActionFile action;
action.m_header.m_boneCount = 10;
action.m_header.m_frameCount = 100;
action.m_header.m_duration = 5.0f;
gobot::GobotFileIO::SaveAction(action, "output.gobot_action");

// Load a scene
gobot::GobotSceneFile scene;
gobot::GobotFileIO::LoadScene("scene.gobot_scene", scene);

// Convert VMD to GoBotAction
gobot::GobotActionFile actionFromVMD;
gobot::GobotConverter::ImportVMDToAction("input.vmd", actionFromVMD);
```

## License

GoBot is part of the Saba MMD Viewer project and distributed under the MIT License.