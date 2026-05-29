//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "ActionClipPlayer.h"
#include "ViewerContext.h"
#include <Saba/GL/Model/MMD/GLMMDModel.h>
#include <Saba/Model/MMD/MMDModel.h>
#include <Saba/Model/MMD/MMDNode.h>
#include <Saba/Base/Log.h>
#include <Saba/Base/Time.h>

#include <algorithm>
#include <cmath>

namespace saba
{
    ActionClipPlayer::ActionClipPlayer()
        : m_modelIndex(-1)
        , m_model(nullptr)
        , m_splitter(nullptr)
        , m_recorder(nullptr)
        , m_state(ClipPlaybackState::Stopped)
        , m_paused(false)
        , m_currentSegmentIndex(-1)
        , m_segmentLocalTime(0.0)
        , m_totalPlaybackTime(0.0)
        , m_gapWaitTime(0.0)
        , m_lastUpdateTime(0.0)
        , m_firstUpdate(true)
        , m_cachedDuration(0.0)
    {
    }

    ActionClipPlayer::~ActionClipPlayer()
    {
        Clear();
    }

    void ActionClipPlayer::SetModel(int modelIndex, GLMMDModel* model)
    {
        m_modelIndex = modelIndex;
        m_model = model;
    }

    void ActionClipPlayer::Clear()
    {
        m_modelIndex = -1;
        m_model = nullptr;
        m_state = ClipPlaybackState::Stopped;
        m_paused = false;
        m_currentSegmentIndex = -1;
        m_segmentLocalTime = 0.0;
        m_totalPlaybackTime = 0.0;
        m_gapWaitTime = 0.0;
        m_firstUpdate = true;
    }

    void ActionClipPlayer::PlaySegment(int segmentIndex)
    {
        if (m_splitter == nullptr)
        {
            SABA_WARN("ActionClipPlayer: No splitter set.");
            return;
        }

        const auto& segments = m_splitter->GetSegments();
        if (segmentIndex < 0 || segmentIndex >= static_cast<int>(segments.size()))
        {
            SABA_WARN("ActionClipPlayer: Invalid segment index {}.", segmentIndex);
            return;
        }

        m_state = ClipPlaybackState::PlayingSegment;
        m_paused = false;
        m_currentSegmentIndex = segmentIndex;
        m_segmentLocalTime = segments[segmentIndex].m_startTime;
        m_totalPlaybackTime = 0.0;
        m_gapWaitTime = 0.0;
        m_firstUpdate = true;

        SABA_INFO("ActionClipPlayer: Playing segment '{}' (index {}).",
                  segments[segmentIndex].m_name, segmentIndex);
    }

    void ActionClipPlayer::PlayAll()
    {
        if (m_splitter == nullptr)
        {
            SABA_WARN("ActionClipPlayer: No splitter set.");
            return;
        }

        const auto& segments = m_splitter->GetSegments();
        if (segments.empty())
        {
            SABA_WARN("ActionClipPlayer: No segments to play.");
            return;
        }

        m_state = ClipPlaybackState::PlayingSegment;
        m_paused = false;
        m_currentSegmentIndex = 0;
        m_segmentLocalTime = segments[0].m_startTime;
        m_totalPlaybackTime = 0.0;
        m_gapWaitTime = 0.0;
        m_firstUpdate = true;

        SABA_INFO("ActionClipPlayer: Playing all {} segments sequentially.",
                  segments.size());
    }

    void ActionClipPlayer::Stop()
    {
        m_state = ClipPlaybackState::Stopped;
        m_paused = false;
        m_currentSegmentIndex = -1;
        m_segmentLocalTime = 0.0;
        m_totalPlaybackTime = 0.0;
        m_gapWaitTime = 0.0;

        if (m_model != nullptr)
        {
            m_model->ResetAnimation();
        }
    }

    void ActionClipPlayer::Pause()
    {
        m_paused = true;
    }

    void ActionClipPlayer::Resume()
    {
        m_paused = false;
        m_firstUpdate = true;
    }

    bool ActionClipPlayer::IsPlaying() const
    {
        return m_state == ClipPlaybackState::PlayingSegment ||
               m_state == ClipPlaybackState::WaitingGap;
    }

    bool ActionClipPlayer::IsPaused() const
    {
        return m_paused;
    }

    double ActionClipPlayer::GetPlaybackTime() const
    {
        return m_totalPlaybackTime;
    }

    RecordedFrame ActionClipPlayer::InterpolateFrame(
        const RecordedFrame& a, const RecordedFrame& b, double t) const
    {
        RecordedFrame result;
        result.m_time = a.m_time + t * (b.m_time - a.m_time);

        for (const auto& boneA : a.m_boneFrames)
        {
            auto it = std::find_if(b.m_boneFrames.begin(), b.m_boneFrames.end(),
                [&boneA](const RecordedBoneFrame& bf) {
                    return bf.m_boneName == boneA.m_boneName;
                });

            if (it != b.m_boneFrames.end())
            {
                RecordedBoneFrame interp;
                interp.m_boneName = boneA.m_boneName;
                interp.m_translate = glm::mix(a.m_translate, it->m_translate, static_cast<float>(t));
                interp.m_rotate = glm::slerp(a.m_rotate, it->m_rotate, static_cast<float>(t));
                result.m_boneFrames.push_back(interp);
            }
            else
            {
                result.m_boneFrames.push_back(boneA);
            }
        }

        return result;
    }

    void ActionClipPlayer::ApplyFrameToModel(const RecordedFrame& frame, MMDModel* mmdModel)
    {
        if (mmdModel == nullptr)
        {
            return;
        }

        auto nodeMan = mmdModel->GetNodeManager();
        if (nodeMan == nullptr)
        {
            return;
        }

        for (const auto& boneFrame : frame.m_boneFrames)
        {
            MMDNode* node = nodeMan->GetMMDNode(boneFrame.m_boneName);
            if (node != nullptr)
            {
                node->SetAnimationTranslate(boneFrame.m_translate);
                node->SetAnimationRotate(boneFrame.m_rotate);
            }
        }
    }

    void ActionClipPlayer::Update(ViewerContext* ctxt)
    {
        if (m_model == nullptr || m_splitter == nullptr || m_recorder == nullptr)
        {
            return;
        }

        if (!IsPlaying() || m_paused)
        {
            return;
        }

        const auto& segments = m_splitter->GetSegments();
        if (segments.empty())
        {
            Stop();
            return;
        }

        double currentTime = GetTime();
        if (m_firstUpdate)
        {
            m_lastUpdateTime = currentTime;
            m_firstUpdate = false;
            return;
        }

        double deltaTime = currentTime - m_lastUpdateTime;
        m_lastUpdateTime = currentTime;

        if (deltaTime > 0.1)
        {
            deltaTime = 0.1;
        }

        const auto& frames = m_recorder->GetFrames();
        if (frames.empty())
        {
            Stop();
            return;
        }

        MMDModel* mmdModel = m_model->GetMMDModel();
        if (mmdModel == nullptr)
        {
            return;
        }

        if (m_state == ClipPlaybackState::PlayingSegment)
        {
            if (m_currentSegmentIndex >= static_cast<int>(segments.size()))
            {
                m_state = ClipPlaybackState::Finished;
                return;
            }

            const auto& seg = segments[m_currentSegmentIndex];

            m_segmentLocalTime += deltaTime;
            m_totalPlaybackTime += deltaTime;

            double segEndAbs = seg.m_startTime + seg.m_duration;

            if (m_segmentLocalTime >= segEndAbs)
            {
                m_segmentLocalTime = segEndAbs;

                if (!frames.empty())
                {
                    ApplyFrameToModel(frames.back(), mmdModel);
                }

                if (seg.m_gapAfter > 0.0)
                {
                    m_state = ClipPlaybackState::WaitingGap;
                    m_gapWaitTime = 0.0;
                }
                else
                {
                    m_currentSegmentIndex++;
                    if (m_currentSegmentIndex >= static_cast<int>(segments.size()))
                    {
                        m_state = ClipPlaybackState::Finished;
                    }
                    else
                    {
                        m_segmentLocalTime = segments[m_currentSegmentIndex].m_startTime;
                    }
                }
            }
            else
            {
                size_t frameA = 0;
                size_t frameB = frames.size() - 1;

                for (size_t fi = 0; fi + 1 < frames.size(); fi++)
                {
                    if (frames[fi + 1].m_time >= m_segmentLocalTime)
                    {
                        frameA = fi;
                        frameB = fi + 1;
                        break;
                    }
                }

                const auto& fa = frames[frameA];
                const auto& fb = frames[frameB];

                double frameDuration = fb.m_time - fa.m_time;
                if (frameDuration > 0.0)
                {
                    double t = (m_segmentLocalTime - fa.m_time) / frameDuration;
                    t = glm::clamp(t, 0.0, 1.0);

                    RecordedFrame interp = InterpolateFrame(fa, fb, t);
                    ApplyFrameToModel(interp, mmdModel);
                }
                else
                {
                    ApplyFrameToModel(fa, mmdModel);
                }
            }
        }
        else if (m_state == ClipPlaybackState::WaitingGap)
        {
            m_gapWaitTime += deltaTime;
            m_totalPlaybackTime += deltaTime;

            const auto& seg = segments[m_currentSegmentIndex];
            if (m_gapWaitTime >= seg.m_gapAfter)
            {
                m_currentSegmentIndex++;
                if (m_currentSegmentIndex >= static_cast<int>(segments.size()))
                {
                    m_state = ClipPlaybackState::Finished;
                }
                else
                {
                    m_state = ClipPlaybackState::PlayingSegment;
                    m_segmentLocalTime = segments[m_currentSegmentIndex].m_startTime;
                }
            }
        }

        if (m_state == ClipPlaybackState::Finished)
        {
            SABA_INFO("ActionClipPlayer: Playback finished.");
        }
    }
}