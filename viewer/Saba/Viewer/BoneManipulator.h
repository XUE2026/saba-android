//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_BONEMANIPULATOR_H_
#define SABA_VIEWER_BONEMANIPULATOR_H_

#include <string>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace saba
{
	struct BoneOverride
	{
		glm::quat rotation;
		glm::vec3 translation;
	};

	class BoneManipulator
	{
	public:
		BoneManipulator();
		~BoneManipulator();

		void SetBoneRotation(const std::string& boneName, const glm::quat& rotation);
		void SetBoneTranslation(const std::string& boneName, const glm::vec3& translation);
		void ResetBone(const std::string& boneName);
		void ResetAll();

		glm::quat GetBoneRotation(const std::string& boneName) const;
		glm::vec3 GetBoneTranslation(const std::string& boneName) const;

		bool HasOverride(const std::string& boneName) const;
		const std::unordered_map<std::string, BoneOverride>& GetAllOverrides() const;

	private:
		std::unordered_map<std::string, BoneOverride> m_overrides;
	};
}

#endif // !SABA_VIEWER_BONEMANIPULATOR_H_