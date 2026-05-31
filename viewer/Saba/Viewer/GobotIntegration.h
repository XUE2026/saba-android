//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_GOBOTINTEGRATION_H_
#define SABA_VIEWER_GOBOTINTEGRATION_H_

#include "ActionRecorder.h"
#include "EnvironmentManager.h"

#include <string>
#include <vector>

namespace saba
{
	class GobotIntegration
	{
	public:
		GobotIntegration();
		~GobotIntegration();

		bool LoadGobotAction(const std::string& path, std::vector<BoneTransform>& bones, CameraTransform& cam);
		bool SaveGobotAction(const std::string& path, const std::vector<BoneTransform>& bones, const CameraTransform& cam);
		bool LoadGobotScene(const std::string& path, EnvironmentManager& env);
		bool SaveGobotScene(const std::string& path, const EnvironmentManager& env);
	};
}

#endif // !SABA_VIEWER_GOBOTINTEGRATION_H_