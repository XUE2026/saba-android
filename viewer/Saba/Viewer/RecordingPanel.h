//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_RECORDINGPANEL_H_
#define SABA_VIEWER_RECORDINGPANEL_H_

#include "ActionRecorder.h"
#include "ActionSplitter.h"
#include "ActionClipPlayer.h"

#include <memory>
#include <string>

namespace saba
{
    class GLMMDModel;
    class ViewerContext;

    class RecordingPanel
    {
    public:
        RecordingPanel();
        ~RecordingPanel();

        RecordingPanel(const RecordingPanel&) = delete;
        RecordingPanel& operator=(const RecordingPanel&) = delete;

        void SetModel(int modelIndex, GLMMDModel* model);
        void ClearModel();

        void Update(ViewerContext* ctxt);
        void Draw();

        ActionRecorder* GetRecorder() { return m_recorder.get(); }
        ActionSplitter* GetSplitter() { return m_splitter.get(); }
        ActionClipPlayer* GetPlayer() { return m_player.get(); }

        bool IsPanelOpen() const { return m_panelOpen; }

    private:
        void DrawRecordingSection();
        void DrawLoadSection();
        void DrawSplittingSection();
        void DrawSegmentList();
        void DrawPlayerSection();

        void OnStartRecording();
        void OnStopRecording();
        void OnSaveRecording();
        void OnLoadRecording();
        void OnAutoSplit();
        void OnMergeAll();
        void OnExportVMD();
        void OnPlaySegment(int segmentIndex);
        void OnPlayAll();
        void OnStopPlayback();

        int                     m_modelIndex;
        GLMMDModel*             m_model;

        std::unique_ptr<ActionRecorder>     m_recorder;
        std::unique_ptr<ActionSplitter>     m_splitter;
        std::unique_ptr<ActionClipPlayer>   m_player;

        bool    m_panelOpen;

        char    m_savePath[256];
        char    m_loadPath[256];

        double  m_recordingStartTime;
        int     m_playingSegmentIndex;
    };
}

#endif // !SABA_VIEWER_RECORDINGPANEL_H_