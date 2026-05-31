#include "GobotConverter.h"
#include "GobotFileIO.h"
#include <Saba/Model/MMD/VMDFile.h>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>

bool VMDToGobotAction(const char* vmdPath, const char* gobotPath) {
    saba::VMDFile vmd;
    if (!saba::ReadVMDFile(vmdPath, &vmd)) {
        return false;
    }

    GobotActionHeader header;
    header.numBoneFrames = static_cast<uint32_t>(vmd.m_boneFrames.size());
    header.numCameraFrames = static_cast<uint32_t>(vmd.m_cameraFrames.size());
    header.numMorphFrames = static_cast<uint32_t>(vmd.m_morphFrames.size());

    std::vector<GobotBoneFrame> bones(header.numBoneFrames);
    for (uint32_t i = 0; i < header.numBoneFrames; ++i) {
        const auto& src = vmd.m_boneFrames[i];
        auto& dst = bones[i];
        std::memset(dst.boneName, 0, sizeof(dst.boneName));
        std::strncpy(dst.boneName, src.m_boneName.c_str(), sizeof(dst.boneName) - 1);
        dst.translation[0] = src.m_translate[0];
        dst.translation[1] = src.m_translate[1];
        dst.translation[2] = src.m_translate[2];
        dst.rotation[0] = src.m_quaternion[0];
        dst.rotation[1] = src.m_quaternion[1];
        dst.rotation[2] = src.m_quaternion[2];
        dst.rotation[3] = src.m_quaternion[3];
        dst.frameIndex = src.m_frameIndex;
    }

    std::vector<GobotCameraFrame> cameras(header.numCameraFrames);
    for (uint32_t i = 0; i < header.numCameraFrames; ++i) {
        const auto& src = vmd.m_cameraFrames[i];
        auto& dst = cameras[i];
        dst.position[0] = src.m_position[0];
        dst.position[1] = src.m_position[1];
        dst.position[2] = src.m_position[2];
        dst.rotation[0] = src.m_rotation[0];
        dst.rotation[1] = src.m_rotation[1];
        dst.rotation[2] = src.m_rotation[2];
        dst.distance = src.m_distance;
        dst.fov = src.m_fov;
        dst.frameIndex = src.m_frameIndex;
    }

    std::vector<GobotMorphFrame> morphs(header.numMorphFrames);
    for (uint32_t i = 0; i < header.numMorphFrames; ++i) {
        const auto& src = vmd.m_morphFrames[i];
        auto& dst = morphs[i];
        std::memset(dst.morphName, 0, sizeof(dst.morphName));
        std::strncpy(dst.morphName, src.m_morphName.c_str(), sizeof(dst.morphName) - 1);
        dst.weight = src.m_weight;
        dst.frameIndex = src.m_frameIndex;
    }

    return WriteGobotAction(gobotPath, header, bones.data(), cameras.data(), morphs.data());
}

bool GobotActionToVMD(const char* gobotPath, const char* vmdPath) {
    GobotActionHeader header;
    std::vector<GobotBoneFrame> bones;
    std::vector<GobotCameraFrame> cameras;
    std::vector<GobotMorphFrame> morphs;

    if (!ReadGobotAction(gobotPath, header, bones, cameras, morphs)) {
        return false;
    }

    saba::VMDFile vmd;

    // Convert bone frames
    vmd.m_boneFrames.resize(header.numBoneFrames);
    for (uint32_t i = 0; i < header.numBoneFrames; ++i) {
        const auto& src = bones[i];
        auto& dst = vmd.m_boneFrames[i];
        dst.m_boneName = src.boneName;
        dst.m_translate[0] = src.translation[0];
        dst.m_translate[1] = src.translation[1];
        dst.m_translate[2] = src.translation[2];
        dst.m_quaternion[0] = src.rotation[0];
        dst.m_quaternion[1] = src.rotation[1];
        dst.m_quaternion[2] = src.rotation[2];
        dst.m_quaternion[3] = src.rotation[3];
        dst.m_frameIndex = src.frameIndex;
    }

    // Convert camera frames
    vmd.m_cameraFrames.resize(header.numCameraFrames);
    for (uint32_t i = 0; i < header.numCameraFrames; ++i) {
        const auto& src = cameras[i];
        auto& dst = vmd.m_cameraFrames[i];
        dst.m_position[0] = src.position[0];
        dst.m_position[1] = src.position[1];
        dst.m_position[2] = src.position[2];
        dst.m_rotation[0] = src.rotation[0];
        dst.m_rotation[1] = src.rotation[1];
        dst.m_rotation[2] = src.rotation[2];
        dst.m_distance = src.distance;
        dst.m_fov = src.fov;
        dst.m_frameIndex = src.frameIndex;
    }

    // Convert morph frames
    vmd.m_morphFrames.resize(header.numMorphFrames);
    for (uint32_t i = 0; i < header.numMorphFrames; ++i) {
        const auto& src = morphs[i];
        auto& dst = vmd.m_morphFrames[i];
        dst.m_morphName = src.morphName;
        dst.m_weight = src.weight;
        dst.m_frameIndex = src.frameIndex;
    }

    return saba::WriteVMDFile(vmdPath, &vmd);
}