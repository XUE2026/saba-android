//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "ActionRecorder.h"

#include <Saba/Model/MMD/VMDFile.h>
#include <gobot/GobotTypes.h>
#include <gobot/src/GobotFileIO.h>

#include <cstring>
#include <fstream>

namespace saba
{
	ActionRecorder::ActionRecorder()
		: m_recording(false)
	{
	}

	ActionRecorder::~ActionRecorder()
	{
	}

	void ActionRecorder::StartRecording()
	{
		m_recording = true;
		m_frames.clear();
	}

	void ActionRecorder::StopRecording()
	{
		m_recording = false;
	}

	void ActionRecorder::RecordFrame(const std::vector<BoneTransform>& bones, const CameraTransform& cam)
	{
		if (!m_recording)
		{
			return;
		}

		RecordedFrame frame;
		frame.time = m_frames.empty() ? 0.0 : m_frames.back().time + (1.0 / 30.0);
		frame.bones = bones;
		frame.camera = cam;
		m_frames.push_back(frame);
	}

	bool ActionRecorder::ExportVMD(const std::string& path)
	{
		if (m_frames.empty())
		{
			return false;
		}

		VMDFile vmd;
		vmd.header = VMDFile::Header{};
		std::strncpy(vmd.header.magic, "Vocaloid Motion Data 0002", sizeof(vmd.header.magic));
		vmd.header.modelName[0] = '\0';

		for (size_t i = 0; i < m_frames.size(); i++)
		{
			const auto& frame = m_frames[i];
			for (const auto& bone : frame.bones)
			{
				VMDKeyFrame key;
				std::strncpy(key.boneName, bone.boneName.c_str(), sizeof(key.boneName));
				key.frameIndex = static_cast<uint32_t>(i);
				key.translation[0] = bone.translation.x;
				key.translation[1] = bone.translation.y;
				key.translation[2] = bone.translation.z;
				key.rotation[0] = bone.rotation.x;
				key.rotation[1] = bone.rotation.y;
				key.rotation[2] = bone.rotation.z;
				key.rotation[3] = bone.rotation.w;
				std::memset(key.interpolation, 0, sizeof(key.interpolation));
				vmd.boneFrames.push_back(key);
			}
		}

		VPDFile vpd;
		return vmd.Save(path);
	}

	bool ActionRecorder::ExportGobotAction(const std::string& path)
	{
		if (m_frames.empty())
		{
			return false;
		}

		GobotActionHeader header;
		header.numBoneFrames = 0;

		for (const auto& frame : m_frames)
		{
			header.numBoneFrames += static_cast<uint32_t>(frame.bones.size());
		}
		header.numCameraFrames = static_cast<uint32_t>(m_frames.size());
		header.numMorphFrames = 0;

		std::vector<GobotBoneFrame> boneFrames;
		std::vector<GobotCameraFrame> cameraFrames;

		for (size_t i = 0; i < m_frames.size(); i++)
		{
			const auto& frame = m_frames[i];
			for (const auto& bone : frame.bones)
			{
				GobotBoneFrame bf;
				std::memset(bf.boneName, 0, sizeof(bf.boneName));
				std::strncpy(bf.boneName, bone.boneName.c_str(), sizeof(bf.boneName) - 1);
				bf.translation[0] = bone.translation.x;
				bf.translation[1] = bone.translation.y;
				bf.translation[2] = bone.translation.z;
				bf.rotation[0] = bone.rotation.x;
				bf.rotation[1] = bone.rotation.y;
				bf.rotation[2] = bone.rotation.z;
				bf.rotation[3] = bone.rotation.w;
				bf.frameIndex = static_cast<uint32_t>(i);
				boneFrames.push_back(bf);
			}

			GobotCameraFrame cf;
			cf.position[0] = frame.camera.position.x;
			cf.position[1] = frame.camera.position.y;
			cf.position[2] = frame.camera.position.z;
			cf.rotation[0] = 0.0f;
			cf.rotation[1] = 0.0f;
			cf.rotation[2] = 0.0f;
			cf.distance = glm::length(frame.camera.target - frame.camera.position);
			cf.fov = frame.camera.fov;
			cf.frameIndex = static_cast<uint32_t>(i);
			cameraFrames.push_back(cf);
		}

		return WriteGobotAction(
			path.c_str(),
			header,
			boneFrames.data(),
			cameraFrames.data(),
			nullptr
		);
	}

	size_t ActionRecorder::GetFrameCount() const
	{
		return m_frames.size();
	}

	void ActionRecorder::Clear()
	{
		m_frames.clear();
		m_recording = false;
	}

	const std::vector<RecordedFrame>& ActionRecorder::GetFrames() const
	{
		return m_frames;
	}
}