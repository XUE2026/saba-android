#ifndef GOBOT_TYPES_H
#define GOBOT_TYPES_H

#include <cstdint>
#include <cstring>

#pragma pack(push, 1)

// .gobot_action file format
struct GobotActionHeader {
    char magic[4];
    uint32_t version;
    uint32_t numBoneFrames;
    uint32_t numCameraFrames;
    uint32_t numMorphFrames;
    uint32_t reserved[4];

    GobotActionHeader() {
        magic[0] = 'G'; magic[1] = 'B'; magic[2] = 'A'; magic[3] = 'F';
        version = 1;
        numBoneFrames = 0;
        numCameraFrames = 0;
        numMorphFrames = 0;
        std::memset(reserved, 0, sizeof(reserved));
    }
};

struct GobotBoneFrame {
    char boneName[64];
    float translation[3];
    float rotation[4]; // quaternion x,y,z,w
    uint32_t frameIndex;
};

struct GobotCameraFrame {
    float position[3];
    float rotation[3]; // euler angles
    float distance;
    float fov;
    uint32_t frameIndex;
};

struct GobotMorphFrame {
    char morphName[64];
    float weight;
    uint32_t frameIndex;
};

// .gobot_scene file format
struct GobotSceneHeader {
    char magic[4];
    uint32_t version;
    uint32_t numModels;
    uint32_t numLights;
    uint32_t numGrounds;
    uint32_t numObjects;
    uint32_t reserved[4];

    GobotSceneHeader() {
        magic[0] = 'G'; magic[1] = 'B'; magic[2] = 'S'; magic[3] = 'F';
        version = 1;
        numModels = 0;
        numLights = 0;
        numGrounds = 0;
        numObjects = 0;
        std::memset(reserved, 0, sizeof(reserved));
    }
};

struct GobotSceneModel {
    char modelPath[256];
    char modelName[64];
    float position[3];
    float rotation[4];
    float scale[3];
    uint32_t flags;
};

struct GobotSceneLight {
    float direction[3];
    float color[3];
    float intensity;
    uint32_t type; // 0=hemisphere, 1=directional
};

struct GobotSceneGround {
    uint32_t type; // 0=grid, 1=checker, 2=grass, 3=dirt, 4=asphalt
    float color1[3];
    float color2[3];
    float size;
};

// .gobot_proj file format
struct GobotProjectHeader {
    char magic[4];
    uint32_t version;
    uint32_t numScenes;
    uint32_t numActions;
    uint32_t settingsSize;
    uint32_t reserved[4];

    GobotProjectHeader() {
        magic[0] = 'G'; magic[1] = 'B'; magic[2] = 'P'; magic[3] = 'J';
        version = 1;
        numScenes = 0;
        numActions = 0;
        settingsSize = 0;
        std::memset(reserved, 0, sizeof(reserved));
    }
};

struct GobotProjEntry {
    char name[128];
    uint32_t type; // 0=scene, 1=action
    uint32_t dataSize;
    uint32_t dataOffset;
};

#pragma pack(pop)

#endif // GOBOT_TYPES_H