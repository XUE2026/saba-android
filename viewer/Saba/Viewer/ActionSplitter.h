//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_ACTIONSPLITTER_H_
#define SABA_VIEWER_ACTIONSPLITTER_H_

#include "ActionRecorder.h"

#include <string>
#include <vector>

namespace saba
{
	struct ActionSegment
	{
		std::string name;
		uint32_t startFrame;
		uint32_t endFrame;
		float gapTime;
	};

	class ActionSplitter
	{
	public:
		ActionSplitter();
		~ActionSplitter();

		void SetFrames(const std::vector<RecordedFrame>& frames);
		void Split(float motionStopThreshold = 0.5f);
		size_t GetSegmentCount() const;
		const ActionSegment& GetSegment(int index) const;
		void SetGapTime(int index, float gapTime);
		uint32_t GetSegmentFrameCount(int index) const;
		bool ExportSegmentToVMD(int index, const std::string& path);

	private:
		std::vector<RecordedFrame> m_frames;
		std::vector<ActionSegment> m_segments;
	};
}

#endif // !SABA_VIEWER_ACTIONSPLITTER_H_