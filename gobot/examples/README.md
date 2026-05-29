# GoBot Examples

This directory contains examples showing how .gobot files are structured and used with the Saba MMD Viewer.

## Example 1: Basic Animation (.gobot_action)

The following example demonstrates a simple .gobot_action file with a single bone animation:

```json
{
  "header": {
    "magic": "GOBOT_ACT",
    "version": 1,
    "boneCount": 2,
    "frameCount": 3,
    "duration": 2.0
  },
  "bones": [
    {
      "name": "全ての親",
      "parentIndex": -1
    },
    {
      "name": "センター",
      "parentIndex": 0
    }
  ],
  "frames": [
    {
      "time": 0.0,
      "boneIndex": 1,
      "position": { "x": 0.0, "y": 0.0, "z": 0.0 },
      "rotation": { "x": 0.0, "y": 0.0, "z": 0.0, "w": 1.0 }
    },
    {
      "time": 1.0,
      "boneIndex": 1,
      "position": { "x": 1.0, "y": 0.5, "z": 0.0 },
      "rotation": { "x": 0.0, "y": 0.0, "z": 0.383, "w": 0.924 }
    },
    {
      "time": 2.0,
      "boneIndex": 1,
      "position": { "x": 0.0, "y": 0.0, "z": 0.0 },
      "rotation": { "x": 0.0, "y": 0.0, "z": 0.0, "w": 1.0 }
    }
  ]
}
```

This represents a 2-second animation where the "センター" bone moves from origin to (1, 0.5, 0) with a 45-degree Z rotation and back.

## Example 2: Scene Configuration (.gobot_scene)

```json
{
  "header": {
    "magic": "GOBOT_SCN",
    "version": 1
  },
  "camera": {
    "position": { "x": 0.0, "y": 10.0, "z": -30.0 },
    "target": { "x": 0.0, "y": 5.0, "z": 0.0 },
    "fov": 45.0,
    "nearClip": 0.1,
    "farClip": 1000.0
  },
  "lights": [
    {
      "type": 0,
      "position": { "x": 0.0, "y": 0.0, "z": 0.0 },
      "direction": { "x": 0.5, "y": -1.0, "z": 0.3 },
      "color": { "r": 1.0, "g": 1.0, "b": 1.0, "a": 1.0 },
      "intensity": 1.0
    }
  ],
  "environment": {
    "groundType": 0,
    "ambientColor": { "r": 0.2, "g": 0.2, "b": 0.2, "a": 1.0 },
    "fogColor": { "r": 0.5, "g": 0.5, "b": 0.5, "a": 1.0 },
    "fogDensity": 0.0
  },
  "primitives": [
    {
      "type": 1,
      "size": { "x": 2.0, "y": 2.0, "z": 2.0 },
      "position": { "x": 3.0, "y": 1.0, "z": 0.0 },
      "rotation": { "x": 0.0, "y": 0.0, "z": 0.0 },
      "color": { "r": 1.0, "g": 0.0, "b": 0.0, "a": 1.0 }
    }
  ],
  "filter": {
    "filterType": 0,
    "exposure": 1.0,
    "contrast": 1.0,
    "saturation": 1.0,
    "hueShift": 0.0,
    "temperature": 0.0,
    "tint": 0.0
  }
}
```

This scene places the camera at (0, 10, -30) looking at the origin with a single directional white light, a floor ground type, a red sphere primitive at (3, 1, 0), and no filter effects.

## Example 3: Complete Project (.gobot_proj)

```json
{
  "header": {
    "magic": "GOBOT_PRJ",
    "version": 1
  },
  "models": [
    "models/miku_v2.pmx",
    "models/stage.pmx"
  ],
  "actions": [
    "actions/dance.gobot_action",
    "actions/camera_motion.vmd"
  ],
  "scene": {
    "camera": {
      "position": { "x": 0.0, "y": 8.0, "z": -25.0 },
      "target": { "x": 0.0, "y": 4.0, "z": 0.0 },
      "fov": 35.0,
      "nearClip": 0.1,
      "farClip": 500.0
    },
    "lights": [
      {
        "type": 0,
        "position": { "x": 0.0, "y": 0.0, "z": 0.0 },
        "direction": { "x": 0.3, "y": -0.8, "z": 0.5 },
        "color": { "r": 1.0, "g": 0.95, "b": 0.9, "a": 1.0 },
        "intensity": 1.2
      }
    ],
    "environment": {
      "groundType": 5,
      "ambientColor": { "r": 0.3, "g": 0.3, "b": 0.4, "a": 1.0 },
      "fogColor": { "r": 0.6, "g": 0.7, "b": 0.8, "a": 1.0 },
      "fogDensity": 0.02
    },
    "primitives": [],
    "filter": {
      "filterType": 4,
      "exposure": 1.1,
      "contrast": 1.05,
      "saturation": 1.2,
      "hueShift": 0.0,
      "temperature": 0.0,
      "tint": 0.0
    }
  },
  "uiLayout": {
    "enabled": true,
    "windows": [
      { "name": "ModelList", "x": 10.0, "y": 10.0, "width": 300.0, "height": 400.0 },
      { "name": "AnimationCtrl", "x": 10.0, "y": 420.0, "width": 300.0, "height": 200.0 }
    ]
  },
  "settings": {
    "language": "en",
    "orientation": 1
  }
}
```

This project loads two models and two actions, configures a detailed scene with camera and lighting, sets up UI window layout, and specifies English language with landscape orientation.

## Usage in C++

```cpp
#include <gobot/gobot.h>
#include <gobot/GobotTypes.h>
#include <gobot/GobotFileIO.h>

// Load and inspect a .gobot_action file
gobot::GobotActionFile action;
if (gobot::GobotFileIO::LoadAction("dance.gobot_action", action))
{
    printf("Loaded action: %u bones, %u frames, %.2f seconds\n",
        action.m_header.m_boneCount,
        action.m_header.m_frameCount,
        action.m_header.m_duration);
}

// Create and save a .gobot_scene file
gobot::GobotSceneFile scene;
scene.m_camera.m_position = gobot::GobotVec3(0.0f, 10.0f, -30.0f);
scene.m_camera.m_target = gobot::GobotVec3(0.0f, 0.0f, 0.0f);
gobot::GobotFileIO::SaveScene(scene, "my_scene.gobot_scene");

// Convert VMD to GoBotAction
gobot::GobotActionFile vmdAction;
if (gobot::GobotConverter::ImportVMDToAction("dance.vmd", vmdAction))
{
    gobot::GobotFileIO::SaveAction(vmdAction, "dance_converted.gobot_action");
}
```

Note: The `.gobot_action`, `.gobot_scene`, and `.gobot_proj` files are binary formats. The JSON representations above are for documentation purposes only and illustrate the logical structure of the data.