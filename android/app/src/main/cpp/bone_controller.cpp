#include "bone_controller.h"
#include <android/log.h>
#include <cmath>
#include <algorithm>

#define LOG_TAG "BoneController"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static glm::quat eulerToQuat(const glm::vec3& euler) {
    float cx = cosf(euler.x * 0.5f);
    float sx = sinf(euler.x * 0.5f);
    float cy = cosf(euler.y * 0.5f);
    float sy = sinf(euler.y * 0.5f);
    float cz = cosf(euler.z * 0.5f);
    float sz = sinf(euler.z * 0.5f);

    glm::quat q;
    q.w = cx * cy * cz + sx * sy * sz;
    q.x = sx * cy * cz - cx * sy * sz;
    q.y = cx * sy * cz + sx * cy * sz;
    q.z = cx * cy * sz - sx * sy * cz;
    return q;
}

BoneController::BoneController()
    : mActionTime(0.0f)
    , mCurrentAction(-1) {
    LOGI("BoneController created");
}

BoneController::~BoneController() {
}

void BoneController::setBoneTransform(const std::string& boneName, const glm::vec3& position, const glm::vec3& eulerRotation) {
    BoneState& state = mBones[boneName];
    state.target.position = position;
    state.target.rotation = eulerToQuat(eulerRotation);
    state.current = state.target;
    LOGI("Set bone transform: %s", boneName.c_str());
}

BoneController::BoneTransform BoneController::getBoneTransform(const std::string& boneName) const {
    auto it = mBones.find(boneName);
    if (it != mBones.end()) {
        return it->second.current;
    }
    BoneTransform identity;
    identity.position = glm::vec3(0.0f);
    identity.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    return identity;
}

void BoneController::resetAllBones() {
    for (auto& bone : mBones) {
        bone.second.target.position = glm::vec3(0.0f);
        bone.second.target.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        bone.second.current = bone.second.target;
    }
    LOGI("All bones reset");
}

void BoneController::resetBone(const std::string& boneName) {
    auto it = mBones.find(boneName);
    if (it != mBones.end()) {
        it->second.target.position = glm::vec3(0.0f);
        it->second.target.rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        it->second.current = it->second.target;
    }
}

bool BoneController::hasBone(const std::string& boneName) const {
    return mBones.find(boneName) != mBones.end();
}

std::vector<std::string> BoneController::getBoneNames() const {
    std::vector<std::string> names;
    names.reserve(mBones.size());
    for (const auto& bone : mBones) {
        names.push_back(bone.first);
    }
    return names;
}

void BoneController::applyToModel(void* model) {
    if (!model) {
        LOGE("applyToModel: null model pointer");
        return;
    }
    LOGI("Applying bone transforms to model");
}

void BoneController::playBuiltinAction(int actionIndex, float deltaTime) {
    if (actionIndex < 0 || actionIndex >= Action_Count) {
        LOGE("Invalid action index: %d", actionIndex);
        return;
    }

    mCurrentAction = actionIndex;
    mActionTime += deltaTime;

    switch (actionIndex) {
        case Action_Idle:
            break;
        case Action_Jump:
            updateJump(mActionTime);
            break;
        case Action_Run:
            updateRun(mActionTime);
            break;
        case Action_Walk:
            updateWalk(mActionTime);
            break;
        default:
            break;
    }
}

void BoneController::updateJump(float t) {
    float cycleTime = 1.0f;
    float phase = fmodf(t, cycleTime) / cycleTime;

    BoneState& root = mBones["全ての親"];
    root.target.position = glm::vec3(0.0f);

    float jumpHeight = 0.0f;
    if (phase < 0.5f) {
        float crouchPhase = phase / 0.5f;
        root.target.position.y = -0.1f * crouchPhase;
    } else {
        float jumpPhase = (phase - 0.5f) / 0.5f;
        jumpHeight = 0.4f * sinf(jumpPhase * 3.14159265f) * (1.0f - jumpPhase);
        root.target.position.y = -0.1f + jumpHeight;
    }

    BoneState& leftLeg = mBones["左足"];
    if (phase < 0.5f) {
        float p = phase / 0.5f;
        leftLeg.target.rotation = eulerToQuat(glm::vec3(0.3f * p, 0.0f, 0.0f));
    } else {
        float p = (phase - 0.5f) / 0.5f;
        leftLeg.target.rotation = eulerToQuat(glm::vec3(0.3f * (1.0f - p), 0.0f, 0.0f));
    }

    BoneState& rightLeg = mBones["右足"];
    rightLeg.target.rotation = eulerToQuat(glm::vec3(-leftLeg.target.rotation.x, 0.0f, 0.0f));

    for (auto& bone : mBones) {
        bone.second.current.position += (bone.second.target.position - bone.second.current.position) * 0.3f;
        bone.second.current.rotation = glm::slerp(bone.second.current.rotation, bone.second.target.rotation, 0.3f);
    }
}

void BoneController::updateRun(float t) {
    float cycleTime = 0.5f;
    float phase = fmodf(t, cycleTime) / cycleTime;

    BoneState& root = mBones["全ての親"];
    root.target.position = glm::vec3(0.0f, -0.05f, 0.0f);

    float legSwing = sinf(phase * 2.0f * 3.14159265f) * 0.6f;

    BoneState& leftLeg = mBones["左足"];
    leftLeg.target.rotation = eulerToQuat(glm::vec3(legSwing, 0.0f, 0.0f));

    BoneState& rightLeg = mBones["右足"];
    rightLeg.target.rotation = eulerToQuat(glm::vec3(-legSwing, 0.0f, 0.0f));

    BoneState& leftArm = mBones["左腕"];
    leftArm.target.rotation = eulerToQuat(glm::vec3(0.0f, 0.0f, -legSwing * 0.5f));

    BoneState& rightArm = mBones["右腕"];
    rightArm.target.rotation = eulerToQuat(glm::vec3(0.0f, 0.0f, legSwing * 0.5f));

    float bodyBounce = sinf(phase * 2.0f * 3.14159265f) * 0.02f;
    root.target.position.y += bodyBounce;

    for (auto& bone : mBones) {
        bone.second.current.position += (bone.second.target.position - bone.second.current.position) * 0.5f;
        bone.second.current.rotation = glm::slerp(bone.second.current.rotation, bone.second.target.rotation, 0.5f);
    }
}

void BoneController::updateWalk(float t) {
    float cycleTime = 1.0f;
    float phase = fmodf(t, cycleTime) / cycleTime;

    BoneState& root = mBones["全ての親"];
    root.target.position = glm::vec3(0.0f, -0.02f, 0.0f);

    float legSwing = sinf(phase * 2.0f * 3.14159265f) * 0.3f;

    BoneState& leftLeg = mBones["左足"];
    leftLeg.target.rotation = eulerToQuat(glm::vec3(legSwing, 0.0f, 0.0f));

    BoneState& rightLeg = mBones["右足"];
    rightLeg.target.rotation = eulerToQuat(glm::vec3(-legSwing, 0.0f, 0.0f));

    BoneState& leftArm = mBones["左腕"];
    leftArm.target.rotation = eulerToQuat(glm::vec3(0.0f, 0.0f, -legSwing * 0.3f));

    BoneState& rightArm = mBones["右腕"];
    rightArm.target.rotation = eulerToQuat(glm::vec3(0.0f, 0.0f, legSwing * 0.3f));

    float bodyBounce = sinf(phase * 2.0f * 3.14159265f) * 0.01f;
    root.target.position.y += bodyBounce;

    for (auto& bone : mBones) {
        bone.second.current.position += (bone.second.target.position - bone.second.current.position) * 0.2f;
        bone.second.current.rotation = glm::slerp(bone.second.current.rotation, bone.second.target.rotation, 0.2f);
    }
}