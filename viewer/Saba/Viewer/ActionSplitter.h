//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_ACTIONSPLITTER_H_
#define SABA_VIEWER_ACTIONSPLITTER_H_

#include "ActionRecorder.h"

#include <cstdint>
#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace saba
{
    struct VMDFile;

    struct Segment
    {
        int         m_id;
        std::string m_name;
        double      m_startTime;
        double      m_endTime;
        double      m_duration;
        double      m_gapAfter;

        Segment()
            : m_id(0)
            , m_startTime(0.0)
            , m_endTime(0.0)
            , m_duration(0.0)
            , m_gapAfter(0.5)
        {
        }
    };

    class ActionSplitter
    {
    public:
        ActionSplitter();
        ~ActionSplitter();

        ActionSplitter(const ActionSplitter&) = delete;
        ActionSplitter& operator=(const ActionSplitter&) = delete;

        bool ImportRecording(const std::string& filePath);
        bool ImportFromRecorder(const ActionRecorder& recorder);

        void AutoSplit();
        void ManualSplit(double splitTime);

        const std::vector<Segment>& GetSegments() const { return m_segments; }

        void SetSegmentGap(int segmentIndex, double gapTime);
        void SetGlobalGap(double gapTime);

        int GetSegmentCount() const { return static_cast<int>(m_segments.size()); }

        bool ReorderSegments(const std::vector<int>& newOrder);
        bool RemoveSegment(int index);

        bool MergeToAnimation(VMDFile* outputVmd) const;
        bool ExportAnimation(const std::string& filePath) const;

        void Clear();

    private:
        struct SplitPoint
        {
            double m_time;
        };

        void DetectMotionPauses(std::vector<SplitPoint>& splitPoints) const;
        void BuildSegmentsFromSplits(const std::vector<SplitPoint>& splitPoints);

        static void InterpolateBoneFrames(
            const std::vector<RecordedFrame>& frames,
            double time,
            std::string& outBoneName,
            glm::vec3& outTranslate,
            glm::quat& outRotate
        );

        static uint8_t BezierToInterpolationByte(float t);

        std::vector<Segment>        m_segments;
        std::vector<RecordedFrame>  m_frames;
        std::vector<std::string>    m_boneNames;

        static const double MotionPauseThreshold;
        static const double MotionPauseDuration;
        static const double DefaultGapTime;
    };
}

#endif // !SABA_VIEWER_ACTIONSPLITTER_H_