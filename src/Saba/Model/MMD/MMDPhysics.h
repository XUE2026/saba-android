#ifndef SABA_MODEL_MMD_MMDPHYSICS_H_
#define SABA_MODEL_MMD_MMDPHYSICS_H_

#include "MMDNode.h"
#include "PMDFile.h"
#include "PMXFile.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <memory>

namespace saba {

class MMDModel;

class MMDRigidBody {
public:
    MMDRigidBody() = default;
    ~MMDRigidBody() = default;

    void SetActivation(bool) {}
    void ResetTransform() {}
    void Reset(class MMDPhysics*) {}
    void ReflectGlobalTransform() {}
    void CalcLocalTransform() {}
    glm::mat4 GetTransform() { return glm::mat4(1.0f); }

    bool Create(const PMXRigidbody&, MMDModel*, MMDNode*) { return true; }
    bool Create(const PMDRigidBodyExt&, MMDModel*, MMDNode*) { return true; }
};

class MMDJoint {
public:
    MMDJoint() = default;
    ~MMDJoint() = default;

    bool CreateJoint(const PMXJoint&, MMDRigidBody*, MMDRigidBody*) { return true; }
    bool CreateJoint(const PMDJointExt&, MMDRigidBody*, MMDRigidBody*) { return true; }
};

class MMDPhysics {
public:
    MMDPhysics() = default;
    ~MMDPhysics() = default;

    bool Create() { return true; }
    bool Create(MMDModel* model, const PMXFile& pmx) { return true; }
    bool Create(MMDModel* model, const PMDFile& pmd) { return true; }

    void Destroy() {}

    void AddAnimation(const glm::vec3& translate, const glm::mat4& rotate) {}
    void AddAnimation(const glm::vec3& translate, const glm::quat& rotate) {}
    void AddGroundAnimation(const glm::vec3& translate) {}

    void SetupSimulation() {}
    void Simulate(double elapsed) {}

    void GetBoneMatrix(MMDNode* node) const {}

    void LinkPhysics() {}
    void UpdateBoneMatrix() {}

    void AddRigidBody(MMDRigidBody*) {}
    void RemoveRigidBody(MMDRigidBody*) {}
    void AddJoint(MMDJoint*) {}
    void RemoveJoint(MMDJoint*) {}
    void Update(float) {}
};

} // namespace saba

#endif // SABA_MODEL_MMD_MMDPHYSICS_H_