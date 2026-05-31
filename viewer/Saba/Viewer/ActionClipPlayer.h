//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_ACTIONCLIPPLAYER_H_
#define SABA_VIEWER_ACTIONCLIPPLAYER_H_

#include "ActionRecorder.h"
#include "ActionSplitter.h"

#include <vector>
#include <glm/gtc/quaternion.hpp>

namespace saba
{
	class ActionClipPlayer
	{
	public:
		enum State
		{
			Stopped,
			PlayingSegment,
			WaitingGap,
			Finished
		};

		ActionClipPlayer();
		~ActionClipPlayer();

		void Load(const std::vector<ActionSegment>& segments, const std::vector<RecordedFrame>& frames);
		void Play(int segmentIndex);
		void Stop();
		void Update(float deltaTime);

		State GetState() const;
		int GetCurrentSegment() const;
		float GetCurrentTime() const;
		float GetTotalDuration() const;

		bool GetCurrentBoneTransform(const std::string& boneName, glm::vec3& outTranslation, glm::quat& outRotation) const;

	private:
		std::vector<ActionSegment> m_segments;
		std::vector<RecordedFrame> m_frames;

		State m_state;
		int m_currentSegment;
		float m_currentTime;
		float m_gapTimer;

		void InterpolateFrame(const RecordedFrame& a, const RecordedFrame& b, float t,
			glm::vec3& outTranslation, glm::quat& outRotation) const;
	};
}

#endif // !SABA_VIEWER_ACTIONCLIPPLAYER_H_