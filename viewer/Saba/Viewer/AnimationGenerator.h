//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_ANIMATIONGENERATOR_H_
#define SABA_VIEWER_ANIMATIONGENERATOR_H_

#include "ActionRecorder.h"

#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace saba
{
	class AnimationGenerator
	{
	public:
		enum AnimType
		{
			Idle,
			Walk,
			Run,
			Jump
		};

		AnimationGenerator();
		~AnimationGenerator();

		void Generate(AnimType type, float duration, std::vector<BoneTransform>& output);

		void SetBoneNames(const std::vector<std::string>& boneNames);

	private:
		void GenerateIdle(float duration, std::vector<BoneTransform>& output);
		void GenerateWalk(float duration, std::vector<BoneTransform>& output);
		void GenerateRun(float duration, std::vector<BoneTransform>& output);
		void GenerateJump(float duration, std::vector<BoneTransform>& output);

		std::vector<std::string> m_boneNames;
	};
}

#endif // !SABA_VIEWER_ANIMATIONGENERATOR_H_