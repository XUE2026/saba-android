#ifndef BONE_CONTROLLER_H_
#define BONE_CONTROLLER_H_

#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <map>
#include <string>
#include <vector>

class BoneController {
public:
    struct BoneTransform {
        glm::vec3 position;
        glm::quat rotation;
    };

    BoneController();
    ~BoneController();

    void setBoneTransform(const std::string& boneName, const glm::vec3& position, const glm::vec3& eulerRotation);
    BoneTransform getBoneTransform(const std::string& boneName) const;

    void resetAllBones();
    void resetBone(const std::string& boneName);

    bool hasBone(const std::string& boneName) const;
    int getBoneCount() const { return (int)mBones.size(); }
    std::vector<std::string> getBoneNames() const;

    void applyToModel(void* model);

    void playBuiltinAction(int actionIndex, float deltaTime);

    enum BuiltinAction {
        Action_Idle = 0,
        Action_Jump,
        Action_Run,
        Action_Walk,
        Action_Count
    };

private:
    struct BoneState {
        BoneTransform target;
        BoneTransform current;
        std::string parent;
    };

    std::map<std::string, BoneState> mBones;

    float mActionTime;
    int mCurrentAction;

    void updateJump(float t);
    void updateRun(float t);
    void updateWalk(float t);
};

#endif