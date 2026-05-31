//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "ActionClipPlayer.h"

#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <cmath>

namespace saba
{
	ActionClipPlayer::ActionClipPlayer()
		: m_state(Stopped)
		, m_currentSegment(-1)
		, m_currentTime(0.0f)
		, m_gapTimer(0.0f)
	{
	}

	ActionClipPlayer::~ActionClipPlayer()
	{
	}

	void ActionClipPlayer::Load(const std::vector<ActionSegment>& segments, const std::vector<RecordedFrame>& frames)
	{
		m_segments = segments;
		m_frames = frames;
		m_state = Stopped;
		m_currentSegment = -1;
		m_currentTime = 0.0f;
		m_gapTimer = 0.0f;
	}

	void ActionClipPlayer::Play(int segmentIndex)
	{
		if (segmentIndex < 0 || static_cast<size_t>(segmentIndex) >= m_segments.size())
		{
			return;
		}

		m_currentSegment = segmentIndex;
		m_currentTime = 0.0f;
		m_gapTimer = 0.0f;
		m_state = PlayingSegment;
	}

	void ActionClipPlayer::Stop()
	{
		m_state = Stopped;
		m_currentSegment = -1;
		m_currentTime = 0.0f;
		m_gapTimer = 0.0f;
	}

	void ActionClipPlayer::Update(float deltaTime)
	{
		if (m_state == Stopped || m_state == Finished)
		{
			return;
		}

		if (m_state == WaitingGap)
		{
			m_gapTimer += deltaTime;
			if (m_gapTimer >= m_segments[m_currentSegment].gapTime)
			{
				m_currentSegment++;
				if (static_cast<size_t>(m_currentSegment) >= m_segments.size())
				{
					m_state = Finished;
					return;
				}
				m_currentTime = 0.0f;
				m_gapTimer = 0.0f;
				m_state = PlayingSegment;
			}
			return;
		}

		m_currentTime += deltaTime;

		const ActionSegment& seg = m_segments[m_currentSegment];
		uint32_t frameCount = seg.endFrame - seg.startFrame + 1;
		float frameRate = 30.0f;
		float segmentDuration = static_cast<float>(frameCount) / frameRate;

		if (m_currentTime >= segmentDuration)
		{
			if (m_segments[m_currentSegment].gapTime > 0.0f)
			{
				m_state = WaitingGap;
				m_gapTimer = 0.0f;
			}
			else
			{
				m_currentSegment++;
				if (static_cast<size_t>(m_currentSegment) >= m_segments.size())
				{
					m_state = Finished;
					return;
				}
				m_currentTime = 0.0f;
			}
		}
	}

	ActionClipPlayer::State ActionClipPlayer::GetState() const
	{
		return m_state;
	}

	int ActionClipPlayer::GetCurrentSegment() const
	{
		return m_currentSegment;
	}

	float ActionClipPlayer::GetCurrentTime() const
	{
		return m_currentTime;
	}

	float ActionClipPlayer::GetTotalDuration() const
	{
		float total = 0.0f;
		for (const auto& seg : m_segments)
		{
			uint32_t frameCount = seg.endFrame - seg.startFrame + 1;
			total += static_cast<float>(frameCount) / 30.0f;
			total += seg.gapTime;
		}
		return total;
	}

	bool ActionClipPlayer::GetCurrentBoneTransform(const std::string& boneName, glm::vec3& outTranslation, glm::quat& outRotation) const
	{
		if (m_currentSegment < 0 || static_cast<size_t>(m_currentSegment) >= m_segments.size())
		{
			return false;
		}

		const ActionSegment& seg = m_segments[m_currentSegment];
		uint32_t frameCount = seg.endFrame - seg.startFrame + 1;
		float frameRate = 30.0f;
		float frameTime = m_currentTime * frameRate;
		uint32_t frameIndex = static_cast<uint32_t>(std::floor(frameTime));
		float t = frameTime - static_cast<float>(frameIndex);

		if (frameIndex >= frameCount)
		{
			frameIndex = frameCount - 1;
			t = 0.0f;
		}

		uint32_t actualIndex0 = seg.startFrame + frameIndex;
		uint32_t actualIndex1 = std::min(actualIndex0 + 1, static_cast<uint32_t>(m_frames.size() - 1));

		const RecordedFrame& frame0 = m_frames[actualIndex0];
		const RecordedFrame& frame1 = m_frames[actualIndex1];

		auto findBone = [&](const RecordedFrame& frame) -> std::pair<glm::vec3, glm::quat>
		{
			for (const auto& bt : frame.bones)
			{
				if (bt.boneName == boneName)
				{
					return { bt.translation, bt.rotation };
				}
			}
			return { glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f) };
		};

		auto [trans0, rot0] = findBone(frame0);
		auto [trans1, rot1] = findBone(frame1);

		outTranslation = glm::mix(trans0, trans1, t);
		outRotation = glm::slerp(rot0, rot1, t);

		return true;
	}

	void ActionClipPlayer::InterpolateFrame(const RecordedFrame& a, const RecordedFrame& b, float t,
		glm::vec3& outTranslation, glm::quat& outRotation) const
	{
		outTranslation = glm::mix(a.camera.position, b.camera.position, t);
		outRotation = glm::slerp(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), t);
	}
}