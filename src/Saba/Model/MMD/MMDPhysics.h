#ifndef SABA_MODEL_MMD_MMDPHYSICS_H_
#define SABA_MODEL_MMD_MMDPHYSICS_H_

#include "MMDNode.h"
#include "MMDModel.h"
#include "PMDFile.h"
#include "PMXFile.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <memory>

namespace saba {

class MMDPhysics {
public:
    MMDPhysics() = default;
    ~MMDPhysics() = default;

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
};

} // namespace saba

#endif // SABA_MODEL_MMD_MMDPHYSICS_H_