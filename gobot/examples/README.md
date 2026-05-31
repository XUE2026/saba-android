# GoBot Framework Examples

GoBot defines three custom binary file formats for MMD animation and scene management.

---

## `.gobot_action` (Magic: `GBAF`)

Stores MMD animation data (bone, camera, and morph keyframes).

```json
{
  "header": {
    "magic": "GBAF",
    "version": 1,
    "numBoneFrames": 2,
    "numCameraFrames": 1,
    "numMorphFrames": 1
  },
  "boneFrames": [
    {
      "boneName": "左腕",
      "translation": [0.0, 0.0, 0.0],
      "rotation": [0.0, 0.0, 0.0, 1.0],
      "frameIndex": 0
    },
    {
      "boneName": "左腕",
      "translation": [1.0, 2.0, 0.0],
      "rotation": [0.0, 0.0, 0.707, 0.707],
      "frameIndex": 30
    }
  ],
  "cameraFrames": [
    {
      "position": [0.0, 10.0, 0.0],
      "rotation": [0.0, 0.0, 0.0],
      "distance": 50.0,
      "fov": 30.0,
      "frameIndex": 0
    }
  ],
  "morphFrames": [
    {
      "morphName": "笑い",
      "weight": 0.5,
      "frameIndex": 0
    }
  ]
}
```

| Offset | Size | Field |
|--------|------|-------|
| 0 | 4 | Magic (`GBAF`) |
| 4 | 4 | Version (1) |
| 8 | 4 | numBoneFrames |
| 12 | 4 | numCameraFrames |
| 16 | 4 | numMorphFrames |
| 20 | 16 | reserved[4] |
| 36 | 72×N | GobotBoneFrame[N] |
| ... | 40×M | GobotCameraFrame[M] |
| ... | 72×K | GobotMorphFrame[K] |

### GobotBoneFrame (72 bytes)

| Offset | Size | Field |
|--------|------|-------|
| 0 | 64 | boneName (null-terminated) |
| 64 | 12 | translation[3] (float) |
| 76 | 16 | rotation[4] (float quaternion) |
| 92 | 4 | frameIndex (uint32) |

### GobotCameraFrame (40 bytes)

| Offset | Size | Field |
|--------|------|-------|
| 0 | 12 | position[3] (float) |
| 12 | 12 | rotation[3] (float euler) |
| 24 | 4 | distance (float) |
| 28 | 4 | fov (float) |
| 32 | 4 | frameIndex (uint32) |

### GobotMorphFrame (72 bytes)

| Offset | Size | Field |
|--------|------|-------|
| 0 | 64 | morphName (null-terminated) |
| 64 | 4 | weight (float) |
| 68 | 4 | frameIndex (uint32) |

---

## `.gobot_scene` (Magic: `GBSF`)

Describes a 3D scene: model placements, lighting, and ground settings.

```json
{
  "header": {
    "magic": "GBSF",
    "version": 1,
    "numModels": 1,
    "numLights": 1,
    "numGrounds": 1,
    "numObjects": 0
  },
  "models": [
    {
      "modelPath": "models/miku.pmd",
      "modelName": "初音ミク",
      "position": [0.0, 0.0, 0.0],
      "rotation": [0.0, 0.0, 0.0, 1.0],
      "scale": [1.0, 1.0, 1.0],
      "flags": 0
    }
  ],
  "lights": [
    {
      "direction": [0.5, -1.0, 0.3],
      "color": [1.0, 1.0, 1.0],
      "intensity": 1.0,
      "type": 1
    }
  ],
  "grounds": [
    {
      "type": 0,
      "color1": [0.3, 0.3, 0.3],
      "color2": [0.5, 0.5, 0.5],
      "size": 100.0
    }
  ]
}
```

### GobotSceneModel (336 bytes)

| Offset | Size | Field |
|--------|------|-------|
| 0 | 256 | modelPath (null-terminated) |
| 256 | 64 | modelName (null-terminated) |
| 320 | 12 | position[3] (float) |
| 332 | 16 | rotation[4] (float quaternion) |
| 348 | 12 | scale[3] (float) |
| 360 | 4 | flags (uint32) |

### GobotSceneLight (32 bytes)

| Offset | Size | Field |
|--------|------|-------|
| 0 | 12 | direction[3] (float) |
| 12 | 12 | color[3] (float) |
| 24 | 4 | intensity (float) |
| 28 | 4 | type (uint32) |

### GobotSceneGround (32 bytes)

| Offset | Size | Field |
|--------|------|-------|
| 0 | 4 | type (uint32) |
| 4 | 12 | color1[3] (float) |
| 16 | 12 | color2[3] (float) |
| 28 | 4 | size (float) |

---

## `.gobot_proj` (Magic: `GBPJ`)

Aggregates multiple scenes and actions (motion files) into a single project file.

```json
{
  "header": {
    "magic": "GBPJ",
    "version": 1,
    "numScenes": 1,
    "numActions": 1,
    "settingsSize": 256
  },
  "entries": [
    {
      "name": "main_scene",
      "type": 0,
      "dataSize": 4096,
      "dataOffset": 80
    },
    {
      "name": "dance_action",
      "type": 1,
      "dataSize": 8192,
      "dataOffset": 4176
    }
  ],
  "settings": "<binary settings block>"
}
```

### File Layout

| Offset | Size | Content |
|--------|------|---------|
| 0 | 36 | GobotProjectHeader |
| 36 | 140×N | GobotProjEntry[N] |
| 36+140×N | settingsSize | Raw settings data |

### GobotProjEntry (140 bytes)

| Offset | Size | Field |
|--------|------|-------|
| 0 | 128 | name (null-terminated) |
| 128 | 4 | type (uint32: 0=scene, 1=action) |
| 132 | 4 | dataSize (uint32) |
| 136 | 4 | dataOffset (uint32) |