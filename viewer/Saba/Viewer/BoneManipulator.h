//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_BONEMANIPULATOR_H_
#define SABA_VIEWER_BONEMANIPULATOR_H_

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace saba
{
    class MMDModel;
    class MMDNode;

    struct BoneInfo
    {
        std::string m_name;
        size_t      m_nodeIndex;
    };

    struct ManipulatedBone
    {
        size_t      m_nodeIndex;
        glm::vec3   m_translate;
        glm::quat   m_rotate;
        bool        m_active;
    };

    class BoneManipulator
    {
    public:
        BoneManipulator();

        bool SelectBone(int modelIndex, const std::string& boneName);
        const BoneInfo* GetSelectedBone() const;
        int GetSelectedModelIndex() const { return m_selectedModelIndex; }

        void RotateBone(float angleX, float angleY, float angleZ);
        void TranslateBone(float x, float y, float z);
        void SetBoneRotation(const glm::quat& q);
        void SetBoneTranslation(const glm::vec3& t);

        void ResetBone();
        void ResetAllBones();

        std::vector<std::string> GetBoneList(MMDModel* model) const;

        void Apply(MMDModel* model, float blendWeight = 1.0f);

        const std::map<size_t, ManipulatedBone>& GetOverrides() const { return m_overrides; }
        bool HasOverrides() const { return !m_overrides.empty(); }
        bool IsBoneOverridden(size_t nodeIndex) const;

    private:
        int                          m_selectedModelIndex;
        BoneInfo                     m_selectedBone;
        std::map<size_t, ManipulatedBone> m_overrides;
    };
}

#endif