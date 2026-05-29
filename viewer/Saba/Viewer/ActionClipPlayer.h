//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_ACTIONCLIPPLAYER_H_
#define SABA_VIEWER_ACTIONCLIPPLAYER_H_

#include "ActionSplitter.h"

#include <cstdint>
#include <memory>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace saba
{
    class GLMMDModel;
    class MMDNode;
    class MMDModel;
    class ViewerContext;

    enum class ClipPlaybackState
    {
        Stopped,
        PlayingSegment,
        WaitingGap,
        Finished,
    };

    class ActionClipPlayer
    {
    public:
        ActionClipPlayer();
        ~ActionClipPlayer();

        ActionClipPlayer(const ActionClipPlayer&) = delete;
        ActionClipPlayer& operator=(const ActionClipPlayer&) = delete;

        void SetModel(int modelIndex, GLMMDModel* model);
        void Clear();

        void PlaySegment(int segmentIndex);
        void PlayAll();
        void Stop();
        void Pause();
        void Resume();

        bool IsPlaying() const;
        bool IsPaused() const;
        double GetPlaybackTime() const;

        void SetSplitter(ActionSplitter* splitter) { m_splitter = splitter; }
        void SetRecorder(ActionRecorder* recorder) { m_recorder = recorder; }

        void Update(ViewerContext* ctxt);

    private:
        void ApplyFrameToModel(const RecordedFrame& frame, MMDModel* mmdModel);
        RecordedFrame InterpolateFrame(const RecordedFrame& a, const RecordedFrame& b, double t) const;

        int                     m_modelIndex;
        GLMMDModel*             m_model;

        ActionSplitter*         m_splitter;
        ActionRecorder*         m_recorder;

        ClipPlaybackState       m_state;
        bool                    m_paused;

        int                     m_currentSegmentIndex;
        double                  m_segmentLocalTime;
        double                  m_totalPlaybackTime;
        double                  m_gapWaitTime;

        double                  m_lastUpdateTime;
        bool                    m_firstUpdate;

        double                  m_cachedDuration;
    };
}

#endif // !SABA_VIEWER_ACTIONCLIPPLAYER_H_