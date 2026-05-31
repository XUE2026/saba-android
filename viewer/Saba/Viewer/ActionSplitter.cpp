//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "ActionSplitter.h"

#include <Saba/Model/MMD/VMDFile.h>

#include <cmath>
#include <cstring>
#include <algorithm>

namespace saba
{
	ActionSplitter::ActionSplitter()
	{
	}

	ActionSplitter::~ActionSplitter()
	{
	}

	void ActionSplitter::SetFrames(const std::vector<RecordedFrame>& frames)
	{
		m_frames = frames;
		m_segments.clear();
	}

	void ActionSplitter::Split(float motionStopThreshold)
	{
		m_segments.clear();

		if (m_frames.empty())
		{
			return;
		}

		uint32_t segmentStart = 0;
		int segmentIndex = 0;

		for (size_t i = 1; i < m_frames.size(); i++)
		{
			float totalMotion = 0.0f;
			const auto& prevFrame = m_frames[i - 1];
			const auto& currFrame = m_frames[i];

			for (size_t j = 0; j < prevFrame.bones.size() && j < currFrame.bones.size(); j++)
			{
				float dist = glm::distance(prevFrame.bones[j].translation, currFrame.bones[j].translation);
				float angle = glm::angle(prevFrame.bones[j].rotation, currFrame.bones[j].rotation);
				totalMotion += dist + angle;
			}

			if (totalMotion < motionStopThreshold)
			{
				if (i - 1 > segmentStart)
				{
					ActionSegment seg;
					seg.name = "Segment_" + std::to_string(segmentIndex++);
					seg.startFrame = segmentStart;
					seg.endFrame = static_cast<uint32_t>(i - 1);
					seg.gapTime = 0.0f;
					m_segments.push_back(seg);
				}
				segmentStart = static_cast<uint32_t>(i);
			}
		}

		if (m_frames.size() - 1 > segmentStart)
		{
			ActionSegment seg;
			seg.name = "Segment_" + std::to_string(segmentIndex);
			seg.startFrame = segmentStart;
			seg.endFrame = static_cast<uint32_t>(m_frames.size() - 1);
			seg.gapTime = 0.0f;
			m_segments.push_back(seg);
		}
	}

	size_t ActionSplitter::GetSegmentCount() const
	{
		return m_segments.size();
	}

	const ActionSegment& ActionSplitter::GetSegment(int index) const
	{
		return m_segments[index];
	}

	void ActionSplitter::SetGapTime(int index, float gapTime)
	{
		if (index >= 0 && static_cast<size_t>(index) < m_segments.size())
		{
			m_segments[index].gapTime = gapTime;
		}
	}

	uint32_t ActionSplitter::GetSegmentFrameCount(int index) const
	{
		if (index < 0 || static_cast<size_t>(index) >= m_segments.size())
		{
			return 0;
		}
		return m_segments[index].endFrame - m_segments[index].startFrame + 1;
	}

	bool ActionSplitter::ExportSegmentToVMD(int index, const std::string& path)
	{
		if (index < 0 || static_cast<size_t>(index) >= m_segments.size())
		{
			return false;
		}

		const ActionSegment& seg = m_segments[index];

		VMDFile vmd;
		vmd.header = VMDFile::Header{};
		std::strncpy(vmd.header.magic, "Vocaloid Motion Data 0002", sizeof(vmd.header.magic));
		vmd.header.modelName[0] = '\0';

		for (uint32_t f = seg.startFrame; f <= seg.endFrame; f++)
		{
			const auto& frame = m_frames[f];
			for (const auto& bone : frame.bones)
			{
				VMDKeyFrame key;
				std::strncpy(key.boneName, bone.boneName.c_str(), sizeof(key.boneName));
				key.frameIndex = f - seg.startFrame;
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

		return vmd.Save(path);
	}
}