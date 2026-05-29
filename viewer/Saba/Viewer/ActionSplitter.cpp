//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "ActionSplitter.h"
#include <Saba/Model/MMD/VMDFile.h>
#include <Saba/Model/MMD/VMDAnimation.h>
#include <Saba/Base/Log.h>
#include <Saba/Base/File.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <array>

namespace saba
{
    const double ActionSplitter::MotionPauseThreshold = 0.001;
    const double ActionSplitter::MotionPauseDuration = 0.5;
    const double ActionSplitter::DefaultGapTime = 0.5;

    ActionSplitter::ActionSplitter()
    {
    }

    ActionSplitter::~ActionSplitter()
    {
        Clear();
    }

    bool ActionSplitter::ImportRecording(const std::string& filePath)
    {
        ActionRecorder recorder;
        if (!recorder.LoadRecording(filePath))
        {
            SABA_ERROR("ActionSplitter: Failed to load recording '{}'.", filePath);
            return false;
        }
        return ImportFromRecorder(recorder);
    }

    bool ActionSplitter::ImportFromRecorder(const ActionRecorder& recorder)
    {
        Clear();

        m_frames = recorder.GetFrames();
        if (m_frames.empty())
        {
            SABA_WARN("ActionSplitter: No frames to import.");
            return false;
        }

        SABA_INFO("ActionSplitter: Imported {} frames from recorder.", m_frames.size());
        return true;
    }

    void ActionSplitter::AutoSplit()
    {
        m_segments.clear();

        if (m_frames.empty())
        {
            SABA_WARN("ActionSplitter: No frames to auto-split.");
            return;
        }

        std::vector<SplitPoint> splitPoints;
        DetectMotionPauses(splitPoints);

        BuildSegmentsFromSplits(splitPoints);

        SABA_INFO("ActionSplitter: Auto-split created {} segments.", m_segments.size());
    }

    void ActionSplitter::DetectMotionPauses(std::vector<SplitPoint>& splitPoints) const
    {
        if (m_frames.size() < 2)
        {
            return;
        }

        double pauseStart = -1.0;

        for (size_t i = 1; i < m_frames.size(); i++)
        {
            double dt = m_frames[i].m_time - m_frames[i - 1].m_time;

            double totalMotion = 0.0;
            size_t motionCount = 0;

            const auto& prevBones = m_frames[i - 1].m_boneFrames;
            const auto& currBones = m_frames[i].m_boneFrames;

            for (const auto& currBone : currBones)
            {
                auto it = std::find_if(prevBones.begin(), prevBones.end(),
                    [&currBone](const RecordedBoneFrame& pb) {
                        return pb.m_boneName == currBone.m_boneName;
                    });

                if (it != prevBones.end())
                {
                    double d = glm::distance(it->m_translate, currBone.m_translate);
                    float dot = glm::dot(it->m_rotate, currBone.m_rotate);
                    double angleDelta = 1.0 - glm::abs(dot);
                    totalMotion += d + angleDelta;
                    motionCount++;
                }
            }

            double avgMotion = (motionCount > 0) ? (totalMotion / motionCount) : 0.0;

            if (avgMotion < MotionPauseThreshold)
            {
                if (pauseStart < 0.0)
                {
                    pauseStart = m_frames[i].m_time;
                }
            }
            else
            {
                if (pauseStart >= 0.0)
                {
                    double pauseDuration = m_frames[i].m_time - pauseStart;
                    if (pauseDuration >= MotionPauseDuration)
                    {
                        double splitTime = pauseStart + pauseDuration * 0.5;
                        SplitPoint sp;
                        sp.m_time = splitTime;
                        splitPoints.push_back(sp);
                    }
                    pauseStart = -1.0;
                }
            }
        }
    }

    void ActionSplitter::BuildSegmentsFromSplits(const std::vector<SplitPoint>& splitPoints)
    {
        if (m_frames.empty())
        {
            return;
        }

        double segmentStart = m_frames.front().m_time;

        for (const auto& sp : splitPoints)
        {
            if (sp.m_time <= segmentStart)
            {
                continue;
            }

            if (sp.m_time >= m_frames.back().m_time)
            {
                break;
            }

            Segment seg;
            seg.m_id = static_cast<int>(m_segments.size());
            seg.m_startTime = segmentStart;
            seg.m_endTime = sp.m_time;
            seg.m_duration = seg.m_endTime - seg.m_startTime;
            seg.m_gapAfter = DefaultGapTime;
            seg.m_name = "Segment " + std::to_string(seg.m_id + 1);

            if (seg.m_duration > 0.01)
            {
                m_segments.push_back(seg);
            }

            segmentStart = sp.m_time;
        }

        double lastTime = m_frames.back().m_time;
        if (lastTime - segmentStart > 0.01)
        {
            Segment seg;
            seg.m_id = static_cast<int>(m_segments.size());
            seg.m_startTime = segmentStart;
            seg.m_endTime = lastTime;
            seg.m_duration = seg.m_endTime - seg.m_startTime;
            seg.m_gapAfter = DefaultGapTime;
            seg.m_name = "Segment " + std::to_string(seg.m_id + 1);
            m_segments.push_back(seg);
        }

        if (m_segments.empty())
        {
            Segment seg;
            seg.m_id = 0;
            seg.m_startTime = m_frames.front().m_time;
            seg.m_endTime = m_frames.back().m_time;
            seg.m_duration = seg.m_endTime - seg.m_startTime;
            seg.m_gapAfter = DefaultGapTime;
            seg.m_name = "Segment 1";
            m_segments.push_back(seg);
        }
    }

    void ActionSplitter::ManualSplit(double splitTime)
    {
        if (m_frames.empty())
        {
            return;
        }

        if (splitTime <= m_frames.front().m_time || splitTime >= m_frames.back().m_time)
        {
            SABA_WARN("ActionSplitter: Split time {:.3f} out of range.", splitTime);
            return;
        }

        m_segments.clear();

        std::vector<SplitPoint> splitPoints;
        SplitPoint sp;
        sp.m_time = splitTime;
        splitPoints.push_back(sp);

        BuildSegmentsFromSplits(splitPoints);
    }

    void ActionSplitter::SetSegmentGap(int segmentIndex, double gapTime)
    {
        if (segmentIndex < 0 || segmentIndex >= static_cast<int>(m_segments.size()))
        {
            return;
        }

        gapTime = glm::clamp(gapTime, 0.0, 10.0);
        m_segments[segmentIndex].m_gapAfter = gapTime;
    }

    void ActionSplitter::SetGlobalGap(double gapTime)
    {
        gapTime = glm::clamp(gapTime, 0.0, 10.0);
        for (auto& seg : m_segments)
        {
            seg.m_gapAfter = gapTime;
        }
    }

    bool ActionSplitter::ReorderSegments(const std::vector<int>& newOrder)
    {
        if (newOrder.size() != m_segments.size())
        {
            return false;
        }

        std::vector<Segment> reordered;
        reordered.reserve(m_segments.size());

        for (size_t i = 0; i < newOrder.size(); i++)
        {
            int idx = newOrder[i];
            if (idx < 0 || idx >= static_cast<int>(m_segments.size()))
            {
                return false;
            }
            reordered.push_back(m_segments[idx]);
        }

        for (size_t i = 0; i < reordered.size(); i++)
        {
            reordered[i].m_id = static_cast<int>(i);
        }

        m_segments = std::move(reordered);
        return true;
    }

    bool ActionSplitter::RemoveSegment(int index)
    {
        if (index < 0 || index >= static_cast<int>(m_segments.size()))
        {
            return false;
        }

        m_segments.erase(m_segments.begin() + index);

        for (size_t i = 0; i < m_segments.size(); i++)
        {
            m_segments[i].m_id = static_cast<int>(i);
        }

        return true;
    }

    uint8_t ActionSplitter::BezierToInterpolationByte(float t)
    {
        t = glm::clamp(t, 0.0f, 1.0f);
        return static_cast<uint8_t>(t * 127.0f);
    }

    bool ActionSplitter::MergeToAnimation(VMDFile* outputVmd) const
    {
        if (outputVmd == nullptr || m_frames.empty())
        {
            return false;
        }

        outputVmd->m_motions.clear();

        std::strncpy(outputVmd->m_header.m_header.data(), "Vocaloid Motion Data 0002", 30);
        std::memset(outputVmd->m_header.m_modelName.data(), 0, 20);

        double currentTime = 0.0;
        int nextFrame = 0;

        for (const auto& seg : m_segments)
        {
            double segDuration = seg.m_endTime - seg.m_startTime;

            for (const auto& frame : m_frames)
            {
                if (frame.m_time < seg.m_startTime || frame.m_time > seg.m_endTime)
                {
                    continue;
                }

                double localTime = frame.m_time - seg.m_startTime;
                double absTime = currentTime + localTime;

                int frameNumber = static_cast<int>(std::round(absTime * 30.0));
                if (frameNumber < nextFrame)
                {
                    frameNumber = nextFrame;
                }

                for (const auto& boneFrame : frame.m_boneFrames)
                {
                    VMDMotion motion;
                    std::strncpy(motion.m_boneName.data(), boneFrame.m_boneName.c_str(), 14);
                    motion.m_boneName.data()[14] = '\0';
                    motion.m_frame = static_cast<uint32_t>(frameNumber);
                    motion.m_translate = boneFrame.m_translate;
                    motion.m_quaternion = boneFrame.m_rotate;

                    for (int bi = 0; bi < 4; bi++)
                    {
                        motion.m_interpolation[bi * 16 + 0] = BezierToInterpolationByte(0.0f);
                        motion.m_interpolation[bi * 16 + 1] = BezierToInterpolationByte(0.0f);
                        motion.m_interpolation[bi * 16 + 2] = BezierToInterpolationByte(1.0f);
                        motion.m_interpolation[bi * 16 + 3] = BezierToInterpolationByte(1.0f);
                    }

                    outputVmd->m_motions.push_back(motion);
                }

                nextFrame = frameNumber + 1;
            }

            currentTime += segDuration + seg.m_gapAfter;
            nextFrame = static_cast<int>(std::round(currentTime * 30.0)) + 1;
        }

        SABA_INFO("ActionSplitter: Merged {} segments into animation with {} keyframes.",
                  m_segments.size(), outputVmd->m_motions.size());
        return true;
    }

    bool ActionSplitter::ExportAnimation(const std::string& filePath) const
    {
        VMDFile vmd;
        if (!MergeToAnimation(&vmd))
        {
            SABA_ERROR("ActionSplitter: Failed to merge segments.");
            return false;
        }

        File file;
        if (!file.Create(filePath.c_str()))
        {
            SABA_ERROR("ActionSplitter: Failed to create file '{}'.", filePath);
            return false;
        }

        if (!file.Write(&vmd.m_header.m_header) || !file.Write(&vmd.m_header.m_modelName))
        {
            SABA_ERROR("ActionSplitter: Failed to write VMD header.");
            return false;
        }

        uint32_t motionCount = static_cast<uint32_t>(vmd.m_motions.size());
        if (!file.Write(&motionCount))
        {
            return false;
        }

        for (const auto& motion : vmd.m_motions)
        {
            if (!file.Write(const_cast<VMDString<15>*>(&motion.m_boneName)))
            {
                return false;
            }
            if (!file.Write(const_cast<uint32_t*>(&motion.m_frame)))
            {
                return false;
            }
            if (!file.Write(const_cast<glm::vec3*>(&motion.m_translate)))
            {
                return false;
            }
            if (!file.Write(const_cast<glm::quat*>(&motion.m_quaternion)))
            {
                return false;
            }
            if (!file.Write(const_cast<std::array<uint8_t, 64>*>(&motion.m_interpolation)))
            {
                return false;
            }
        }

        uint32_t zero = 0;
        file.Write(&zero);
        file.Write(&zero);
        file.Write(&zero);
        file.Write(&zero);
        file.Write(&zero);

        SABA_INFO("ActionSplitter: Exported VMD animation to '{}' ({} keyframes).",
                  filePath, motionCount);
        return true;
    }

    void ActionSplitter::Clear()
    {
        m_segments.clear();
        m_frames.clear();
        m_boneNames.clear();
    }
}