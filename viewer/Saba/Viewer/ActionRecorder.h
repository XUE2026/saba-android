//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_ACTIONRECORDER_H_
#define SABA_VIEWER_ACTIONRECORDER_H_

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace saba
{
    class GLMMDModel;
    class MMDNode;

    struct RecordedBoneFrame
    {
        std::string     m_boneName;
        glm::vec3       m_translate;
        glm::quat       m_rotate;
    };

    struct RecordedFrame
    {
        double                          m_time;
        std::vector<RecordedBoneFrame>  m_boneFrames;
    };

    class ActionRecorder
    {
    public:
        ActionRecorder();
        ~ActionRecorder();

        ActionRecorder(const ActionRecorder&) = delete;
        ActionRecorder& operator=(const ActionRecorder&) = delete;

        void StartRecording(int modelIndex, GLMMDModel* model);
        void StopRecording();
        bool IsRecording() const;

        void RecordFrame(double time);

        double GetRecordingDuration() const;
        size_t GetRecordedBoneCount() const;
        int GetModelIndex() const { return m_modelIndex; }

        const std::vector<RecordedFrame>& GetFrames() const { return m_frames; }

        bool SaveRecording(const std::string& filePath) const;
        bool LoadRecording(const std::string& filePath);

        void Clear();

        static const uint32_t RecordFileMagic = 0x4241535f; // "SABA_REC"
        static const uint32_t RecordFileVersion = 0x00000001;
        static const double   RecordFPS;
        static const double   RecordInterval;

    private:
        void CaptureCurrentBonePose();

        int             m_modelIndex;
        GLMMDModel*     m_model;
        bool            m_isRecording;
        double          m_lastRecordTime;

        std::vector<std::string>    m_boneNames;
        std::vector<RecordedFrame>  m_frames;

        struct BoneState
        {
            glm::vec3   m_translate;
            glm::quat   m_rotate;
        };
        std::map<std::string, BoneState> m_lastBoneState;
    };
}

#endif // !SABA_VIEWER_ACTIONRECORDER_H_