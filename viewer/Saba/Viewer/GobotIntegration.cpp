//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "GobotIntegration.h"

#include <gobot/GobotTypes.h>
#include <gobot/src/GobotFileIO.h>
#include <gobot/src/GobotConverter.h>

#include <cstring>
#include <glm/gtc/quaternion.hpp>

namespace saba
{
	GobotIntegration::GobotIntegration()
	{
	}

	GobotIntegration::~GobotIntegration()
	{
	}

	bool GobotIntegration::LoadGobotAction(const std::string& path, std::vector<BoneTransform>& bones, CameraTransform& cam)
	{
		GobotActionHeader header;
		std::vector<GobotBoneFrame> boneFrames;
		std::vector<GobotCameraFrame> cameraFrames;
		std::vector<GobotMorphFrame> morphFrames;

		if (!ReadGobotAction(path.c_str(), header, boneFrames, cameraFrames, morphFrames))
		{
			return false;
		}

		bones.clear();
		for (const auto& bf : boneFrames)
		{
			BoneTransform bt;
			bt.boneName = bf.boneName;
			bt.translation = glm::vec3(bf.translation[0], bf.translation[1], bf.translation[2]);
			bt.rotation = glm::quat(bf.rotation[3], bf.rotation[0], bf.rotation[1], bf.rotation[2]);
			bones.push_back(bt);
		}

		if (!cameraFrames.empty())
		{
			const auto& cf = cameraFrames[0];
			cam.position = glm::vec3(cf.position[0], cf.position[1], cf.position[2]);
			cam.target = glm::vec3(0.0f);
			cam.up = glm::vec3(0.0f, 1.0f, 0.0f);
			cam.fov = cf.fov;
		}

		return true;
	}

	bool GobotIntegration::SaveGobotAction(const std::string& path, const std::vector<BoneTransform>& bones, const CameraTransform& cam)
	{
		GobotActionHeader header;
		header.numBoneFrames = static_cast<uint32_t>(bones.size());
		header.numCameraFrames = 1;
		header.numMorphFrames = 0;

		std::vector<GobotBoneFrame> boneFrames;
		boneFrames.reserve(bones.size());
		for (const auto& bt : bones)
		{
			GobotBoneFrame bf;
			std::memset(bf.boneName, 0, sizeof(bf.boneName));
			std::strncpy(bf.boneName, bt.boneName.c_str(), sizeof(bf.boneName) - 1);
			bf.translation[0] = bt.translation.x;
			bf.translation[1] = bt.translation.y;
			bf.translation[2] = bt.translation.z;
			bf.rotation[0] = bt.rotation.x;
			bf.rotation[1] = bt.rotation.y;
			bf.rotation[2] = bt.rotation.z;
			bf.rotation[3] = bt.rotation.w;
			bf.frameIndex = 0;
			boneFrames.push_back(bf);
		}

		GobotCameraFrame cf;
		cf.position[0] = cam.position.x;
		cf.position[1] = cam.position.y;
		cf.position[2] = cam.position.z;
		cf.rotation[0] = 0.0f;
		cf.rotation[1] = 0.0f;
		cf.rotation[2] = 0.0f;
		cf.distance = glm::length(cam.target - cam.position);
		cf.fov = cam.fov;
		cf.frameIndex = 0;

		return WriteGobotAction(
			path.c_str(),
			header,
			boneFrames.data(),
			&cf,
			nullptr
		);
	}

	bool GobotIntegration::LoadGobotScene(const std::string& path, EnvironmentManager& env)
	{
		GobotSceneHeader header;
		std::vector<GobotSceneModel> models;
		std::vector<GobotSceneLight> lights;
		std::vector<GobotSceneGround> grounds;

		if (!ReadGobotScene(path.c_str(), header, models, lights, grounds))
		{
			return false;
		}

		if (!lights.empty())
		{
			SunParams sun;
			sun.direction = glm::vec3(
				lights[0].direction[0],
				lights[0].direction[1],
				lights[0].direction[2]
			);
			sun.color = glm::vec3(
				lights[0].color[0],
				lights[0].color[1],
				lights[0].color[2]
			);
			sun.intensity = lights[0].intensity;
			env.SetSun(sun);
		}

		if (!grounds.empty())
		{
			GroundParams ground;
			ground.type = static_cast<GroundType>(grounds[0].type);
			ground.color1 = glm::vec3(grounds[0].color1[0], grounds[0].color1[1], grounds[0].color1[2]);
			ground.color2 = glm::vec3(grounds[0].color2[0], grounds[0].color2[1], grounds[0].color2[2]);
			ground.size = grounds[0].size;
			env.SetGround(ground);
		}

		return true;
	}

	bool GobotIntegration::SaveGobotScene(const std::string& path, const EnvironmentManager& env)
	{
		GobotSceneHeader header;
		header.numModels = 0;
		header.numLights = 1;
		header.numGrounds = 1;
		header.numObjects = static_cast<uint32_t>(env.GetObjects().size());

		const SunParams& sun = env.GetSun();
		GobotSceneLight light;
		light.direction[0] = sun.direction.x;
		light.direction[1] = sun.direction.y;
		light.direction[2] = sun.direction.z;
		light.color[0] = sun.color.r;
		light.color[1] = sun.color.g;
		light.color[2] = sun.color.b;
		light.intensity = sun.intensity;
		light.type = 1;

		const GroundParams& ground = env.GetGround();
		GobotSceneGround gd;
		gd.type = static_cast<uint32_t>(ground.type);
		gd.color1[0] = ground.color1.r;
		gd.color1[1] = ground.color1.g;
		gd.color1[2] = ground.color1.b;
		gd.color2[0] = ground.color2.r;
		gd.color2[1] = ground.color2.g;
		gd.color2[2] = ground.color2.b;
		gd.size = ground.size;

		std::vector<GobotSceneModel> models;
		std::vector<GobotSceneLight> lightVec = { light };
		std::vector<GobotSceneGround> groundVec = { gd };

		return WriteGobotScene(
			path.c_str(),
			header,
			models.data(),
			lightVec.data(),
			groundVec.data()
		);
	}
}