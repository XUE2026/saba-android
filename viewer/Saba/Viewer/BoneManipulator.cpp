//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "BoneManipulator.h"
#include <Saba/Model/MMD/MMDModel.h>
#include <Saba/Model/MMD/MMDNode.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace saba
{
    BoneManipulator::BoneManipulator()
        : m_selectedModelIndex(-1)
    {
        m_selectedBone.m_nodeIndex = MMDNodeManager::NPos;
    }

    bool BoneManipulator::SelectBone(int modelIndex, const std::string& boneName)
    {
        m_selectedModelIndex = modelIndex;
        m_selectedBone.m_name = boneName;
        m_selectedBone.m_nodeIndex = MMDNodeManager::NPos;

        return true;
    }

    const BoneInfo* BoneManipulator::GetSelectedBone() const
    {
        if (m_selectedBone.m_nodeIndex == MMDNodeManager::NPos)
        {
            return nullptr;
        }
        return &m_selectedBone;
    }

    void BoneManipulator::RotateBone(float angleX, float angleY, float angleZ)
    {
        if (m_selectedBone.m_nodeIndex == MMDNodeManager::NPos)
        {
            return;
        }

        glm::quat qX = glm::angleAxis(glm::radians(angleX), glm::vec3(1, 0, 0));
        glm::quat qY = glm::angleAxis(glm::radians(angleY), glm::vec3(0, 1, 0));
        glm::quat qZ = glm::angleAxis(glm::radians(angleZ), glm::vec3(0, 0, 1));

        ManipulatedBone& bone = m_overrides[m_selectedBone.m_nodeIndex];
        bone.m_nodeIndex = m_selectedBone.m_nodeIndex;
        bone.m_rotate = qZ * qY * qX;
        bone.m_active = true;
    }

    void BoneManipulator::TranslateBone(float x, float y, float z)
    {
        if (m_selectedBone.m_nodeIndex == MMDNodeManager::NPos)
        {
            return;
        }

        ManipulatedBone& bone = m_overrides[m_selectedBone.m_nodeIndex];
        bone.m_nodeIndex = m_selectedBone.m_nodeIndex;
        bone.m_translate = glm::vec3(x, y, z);
        bone.m_active = true;
    }

    void BoneManipulator::SetBoneRotation(const glm::quat& q)
    {
        if (m_selectedBone.m_nodeIndex == MMDNodeManager::NPos)
        {
            return;
        }

        ManipulatedBone& bone = m_overrides[m_selectedBone.m_nodeIndex];
        bone.m_nodeIndex = m_selectedBone.m_nodeIndex;
        bone.m_rotate = q;
        bone.m_active = true;
    }

    void BoneManipulator::SetBoneTranslation(const glm::vec3& t)
    {
        if (m_selectedBone.m_nodeIndex == MMDNodeManager::NPos)
        {
            return;
        }

        ManipulatedBone& bone = m_overrides[m_selectedBone.m_nodeIndex];
        bone.m_nodeIndex = m_selectedBone.m_nodeIndex;
        bone.m_translate = t;
        bone.m_active = true;
    }

    void BoneManipulator::ResetBone()
    {
        if (m_selectedBone.m_nodeIndex == MMDNodeManager::NPos)
        {
            return;
        }

        auto it = m_overrides.find(m_selectedBone.m_nodeIndex);
        if (it != m_overrides.end())
        {
            m_overrides.erase(it);
        }
    }

    void BoneManipulator::ResetAllBones()
    {
        m_overrides.clear();
    }

    std::vector<std::string> BoneManipulator::GetBoneList(MMDModel* model) const
    {
        std::vector<std::string> boneList;
        if (model == nullptr)
        {
            return boneList;
        }

        auto nodeMan = model->GetNodeManager();
        if (nodeMan == nullptr)
        {
            return boneList;
        }

        size_t nodeCount = nodeMan->GetNodeCount();
        boneList.reserve(nodeCount);
        for (size_t i = 0; i < nodeCount; i++)
        {
            auto node = nodeMan->GetMMDNode(i);
            if (node != nullptr)
            {
                boneList.push_back(node->GetName());
            }
        }

        return boneList;
    }

    void BoneManipulator::Apply(MMDModel* model, float blendWeight)
    {
        if (model == nullptr || m_overrides.empty())
        {
            return;
        }

        auto nodeMan = model->GetNodeManager();
        if (nodeMan == nullptr)
        {
            return;
        }

        for (auto& pair : m_overrides)
        {
            ManipulatedBone& bone = pair.second;
            if (!bone.m_active)
            {
                continue;
            }

            MMDNode* node = nodeMan->GetMMDNode(bone.m_nodeIndex);
            if (node == nullptr)
            {
                continue;
            }

            glm::vec3 animT = node->GetAnimationTranslate();
            glm::quat animQ = node->GetAnimationRotate();

            glm::vec3 targetT = bone.m_translate;
            glm::quat targetQ = bone.m_rotate;

            if (blendWeight < 1.0f)
            {
                targetT = glm::mix(animT, targetT, blendWeight);
                targetQ = glm::slerp(animQ, targetQ, blendWeight);
            }

            node->SetAnimationTranslate(targetT);
            node->SetAnimationRotate(targetQ);
        }
    }

    bool BoneManipulator::IsBoneOverridden(size_t nodeIndex) const
    {
        return m_overrides.find(nodeIndex) != m_overrides.end();
    }
}