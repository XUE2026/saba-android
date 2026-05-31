//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_ACTIONRECORDER_H_
#define SABA_VIEWER_ACTIONRECORDER_H_

#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace saba
{
	struct BoneTransform
	{
		std::string boneName;
		glm::vec3 translation;
		glm::quat rotation;
	};

	struct CameraTransform
	{
		glm::vec3 position;
		glm::vec3 target;
		glm::vec3 up;
		float fov;
	};

	struct RecordedFrame
	{
		double time;
		std::vector<BoneTransform> bones;
		CameraTransform camera;
	};

	class ActionRecorder
	{
	public:
		ActionRecorder();
		~ActionRecorder();

		void StartRecording();
		void StopRecording();
		void RecordFrame(const std::vector<BoneTransform>& bones, const CameraTransform& cam);
		bool ExportVMD(const std::string& path);
		bool ExportGobotAction(const std::string& path);
		size_t GetFrameCount() const;
		void Clear();

		const std::vector<RecordedFrame>& GetFrames() const;

	private:
		bool m_recording;
		std::vector<RecordedFrame> m_frames;
	};
}

#endif // !SABA_VIEWER_ACTIONRECORDER_H_