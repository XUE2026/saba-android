//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "BoneManipulator.h"

namespace saba
{
	BoneManipulator::BoneManipulator()
	{
	}

	BoneManipulator::~BoneManipulator()
	{
	}

	void BoneManipulator::SetBoneRotation(const std::string& boneName, const glm::quat& rotation)
	{
		m_overrides[boneName].rotation = rotation;
	}

	void BoneManipulator::SetBoneTranslation(const std::string& boneName, const glm::vec3& translation)
	{
		m_overrides[boneName].translation = translation;
	}

	void BoneManipulator::ResetBone(const std::string& boneName)
	{
		m_overrides.erase(boneName);
	}

	void BoneManipulator::ResetAll()
	{
		m_overrides.clear();
	}

	glm::quat BoneManipulator::GetBoneRotation(const std::string& boneName) const
	{
		auto it = m_overrides.find(boneName);
		if (it != m_overrides.end())
		{
			return it->second.rotation;
		}
		return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	}

	glm::vec3 BoneManipulator::GetBoneTranslation(const std::string& boneName) const
	{
		auto it = m_overrides.find(boneName);
		if (it != m_overrides.end())
		{
			return it->second.translation;
		}
		return glm::vec3(0.0f);
	}

	bool BoneManipulator::HasOverride(const std::string& boneName) const
	{
		return m_overrides.find(boneName) != m_overrides.end();
	}

	const std::unordered_map<std::string, BoneOverride>& BoneManipulator::GetAllOverrides() const
	{
		return m_overrides;
	}
}