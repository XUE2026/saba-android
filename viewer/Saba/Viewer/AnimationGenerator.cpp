//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "AnimationGenerator.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cmath>
#include <algorithm>

namespace saba
{
	AnimationGenerator::AnimationGenerator()
	{
		m_boneNames = {
			"全ての親",
			"センター",
			"グルーブ",
			"腰",
			"上半身",
			"上半身2",
			"下半身",
			"首",
			"頭",
			"左肩",
			"左腕",
			"左ひじ",
			"左手首",
			"右肩",
			"右腕",
			"右ひじ",
			"右手首",
			"左足",
			"左ひざ",
			"左足首",
			"右足",
			"右ひざ",
			"右足首"
		};
	}

	AnimationGenerator::~AnimationGenerator()
	{
	}

	void AnimationGenerator::SetBoneNames(const std::vector<std::string>& boneNames)
	{
		m_boneNames = boneNames;
	}

	void AnimationGenerator::Generate(AnimType type, float duration, std::vector<BoneTransform>& output)
	{
		output.clear();

		switch (type)
		{
		case Idle:
			GenerateIdle(duration, output);
			break;
		case Walk:
			GenerateWalk(duration, output);
			break;
		case Run:
			GenerateRun(duration, output);
			break;
		case Jump:
			GenerateJump(duration, output);
			break;
		}
	}

	void AnimationGenerator::GenerateIdle(float duration, std::vector<BoneTransform>& output)
	{
		float frameRate = 30.0f;
		int totalFrames = static_cast<int>(duration * frameRate);
		float breathCycle = 2.0f;

		for (int f = 0; f < totalFrames; f++)
		{
			float t = static_cast<float>(f) / frameRate;
			float breath = std::sin(t * glm::two_pi<float>() / breathCycle) * 0.02f;

			BoneTransform bodyBone;
			bodyBone.boneName = "上半身";
			bodyBone.translation = glm::vec3(0.0f, breath, 0.0f);
			bodyBone.rotation = glm::angleAxis(breath * 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(bodyBone);

			BoneTransform headBone;
			headBone.boneName = "頭";
			headBone.translation = glm::vec3(0.0f);
			headBone.rotation = glm::angleAxis(breath * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(headBone);
		}
	}

	void AnimationGenerator::GenerateWalk(float duration, std::vector<BoneTransform>& output)
	{
		float frameRate = 30.0f;
		int totalFrames = static_cast<int>(duration * frameRate);
		float stepTime = 0.5f;
		float legSwingAngle = glm::radians(25.0f);

		for (int f = 0; f < totalFrames; f++)
		{
			float t = static_cast<float>(f) / frameRate;
			float phase = (t / stepTime) * glm::two_pi<float>();
			float swing = std::sin(phase) * legSwingAngle;

			BoneTransform leftLeg;
			leftLeg.boneName = "左足";
			leftLeg.translation = glm::vec3(0.0f);
			leftLeg.rotation = glm::angleAxis(swing, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(leftLeg);

			BoneTransform rightLeg;
			rightLeg.boneName = "右足";
			rightLeg.translation = glm::vec3(0.0f);
			rightLeg.rotation = glm::angleAxis(-swing, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(rightLeg);

			float bodySway = std::sin(phase) * 0.03f;
			BoneTransform bodyBone;
			bodyBone.boneName = "上半身";
			bodyBone.translation = glm::vec3(bodySway, 0.0f, 0.0f);
			bodyBone.rotation = glm::angleAxis(bodySway * 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));
			output.push_back(bodyBone);

			float armSwing = std::sin(phase + glm::pi<float>()) * glm::radians(15.0f);
			BoneTransform leftArm;
			leftArm.boneName = "左腕";
			leftArm.translation = glm::vec3(0.0f);
			leftArm.rotation = glm::angleAxis(armSwing, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(leftArm);

			BoneTransform rightArm;
			rightArm.boneName = "右腕";
			rightArm.translation = glm::vec3(0.0f);
			rightArm.rotation = glm::angleAxis(-armSwing, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(rightArm);
		}
	}

	void AnimationGenerator::GenerateRun(float duration, std::vector<BoneTransform>& output)
	{
		float frameRate = 30.0f;
		int totalFrames = static_cast<int>(duration * frameRate);
		float stepTime = 0.3f;
		float legSwingAngle = glm::radians(40.0f);

		for (int f = 0; f < totalFrames; f++)
		{
			float t = static_cast<float>(f) / frameRate;
			float phase = (t / stepTime) * glm::two_pi<float>();
			float swing = std::sin(phase) * legSwingAngle;

			BoneTransform leftLeg;
			leftLeg.boneName = "左足";
			leftLeg.translation = glm::vec3(0.0f);
			leftLeg.rotation = glm::angleAxis(swing, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(leftLeg);

			BoneTransform rightLeg;
			rightLeg.boneName = "右足";
			rightLeg.translation = glm::vec3(0.0f);
			rightLeg.rotation = glm::angleAxis(-swing, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(rightLeg);

			float bodyBounce = std::abs(std::sin(phase)) * 0.05f;
			BoneTransform bodyBone;
			bodyBone.boneName = "上半身";
			bodyBone.translation = glm::vec3(0.0f, bodyBounce, 0.0f);
			bodyBone.rotation = glm::angleAxis(swing * 0.3f, glm::vec3(0.0f, 0.0f, 1.0f));
			output.push_back(bodyBone);

			float armSwing = std::sin(phase + glm::pi<float>()) * glm::radians(30.0f);
			BoneTransform leftArm;
			leftArm.boneName = "左腕";
			leftArm.translation = glm::vec3(0.0f);
			leftArm.rotation = glm::angleAxis(armSwing, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(leftArm);

			BoneTransform rightArm;
			rightArm.boneName = "右腕";
			rightArm.translation = glm::vec3(0.0f);
			rightArm.rotation = glm::angleAxis(-armSwing, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(rightArm);
		}
	}

	void AnimationGenerator::GenerateJump(float duration, std::vector<BoneTransform>& output)
	{
		float frameRate = 30.0f;
		int totalFrames = static_cast<int>(duration * frameRate);

		float anticipationPhase = 0.15f;
		float launchPhase = 0.1f;
		float risePhase = 0.35f;
		float fallPhase = 0.25f;
		float landingPhase = 0.15f;

		for (int f = 0; f < totalFrames; f++)
		{
			float t = static_cast<float>(f) / static_cast<float>(totalFrames);
			float bodyHeight = 0.0f;
			float bodyTilt = 0.0f;
			float legBend = 0.0f;

			if (t < anticipationPhase)
			{
				float p = t / anticipationPhase;
				bodyHeight = -p * 0.1f;
				legBend = p * glm::radians(30.0f);
				bodyTilt = -p * glm::radians(5.0f);
			}
			else if (t < anticipationPhase + launchPhase)
			{
				float p = (t - anticipationPhase) / launchPhase;
				bodyHeight = -0.1f + p * 0.3f;
				legBend = glm::radians(30.0f) * (1.0f - p);
				bodyTilt = glm::radians(-5.0f) * (1.0f - p);
			}
			else if (t < anticipationPhase + launchPhase + risePhase)
			{
				float p = (t - anticipationPhase - launchPhase) / risePhase;
				float ease = p * p * (3.0f - 2.0f * p);
				bodyHeight = 0.2f + ease * 0.8f;
			}
			else if (t < anticipationPhase + launchPhase + risePhase + fallPhase)
			{
				float p = (t - anticipationPhase - launchPhase - risePhase) / fallPhase;
				float ease = p * p;
				bodyHeight = 1.0f - ease;
				bodyTilt = p * glm::radians(10.0f);
			}
			else
			{
				float p = (t - anticipationPhase - launchPhase - risePhase - fallPhase) / landingPhase;
				bodyHeight = (1.0f - p) * 0.0f;
				legBend = p * glm::radians(20.0f);
				bodyTilt = glm::radians(10.0f) * (1.0f - p);
			}

			BoneTransform bodyBone;
			bodyBone.boneName = "上半身";
			bodyBone.translation = glm::vec3(0.0f, bodyHeight * 0.5f, 0.0f);
			bodyBone.rotation = glm::angleAxis(bodyTilt, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(bodyBone);

			BoneTransform leftLeg;
			leftLeg.boneName = "左足";
			leftLeg.translation = glm::vec3(0.0f);
			leftLeg.rotation = glm::angleAxis(legBend, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(leftLeg);

			BoneTransform rightLeg;
			rightLeg.boneName = "右足";
			rightLeg.translation = glm::vec3(0.0f);
			rightLeg.rotation = glm::angleAxis(legBend, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(rightLeg);

			float armRaise = 0.0f;
			if (t > anticipationPhase && t < anticipationPhase + launchPhase + risePhase + fallPhase)
			{
				float p = (t - anticipationPhase) / (launchPhase + risePhase + fallPhase);
				armRaise = std::sin(p * glm::pi<float>()) * glm::radians(45.0f);
			}

			BoneTransform leftArm;
			leftArm.boneName = "左腕";
			leftArm.translation = glm::vec3(0.0f);
			leftArm.rotation = glm::angleAxis(-armRaise, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(leftArm);

			BoneTransform rightArm;
			rightArm.boneName = "右腕";
			rightArm.translation = glm::vec3(0.0f);
			rightArm.rotation = glm::angleAxis(-armRaise, glm::vec3(1.0f, 0.0f, 0.0f));
			output.push_back(rightArm);
		}
	}
}