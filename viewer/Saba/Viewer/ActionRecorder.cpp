//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "ActionRecorder.h"
#include <Saba/GL/Model/MMD/GLMMDModel.h>
#include <Saba/Model/MMD/MMDModel.h>
#include <Saba/Model/MMD/MMDNode.h>
#include <Saba/Base/Log.h>
#include <Saba/Base/File.h>

#include <cstring>
#include <algorithm>

namespace saba
{
    const double ActionRecorder::RecordFPS = 30.0;
    const double ActionRecorder::RecordInterval = 1.0 / RecordFPS;

    ActionRecorder::ActionRecorder()
        : m_modelIndex(-1)
        , m_model(nullptr)
        , m_isRecording(false)
        , m_lastRecordTime(0.0)
    {
    }

    ActionRecorder::~ActionRecorder()
    {
        Clear();
    }

    void ActionRecorder::StartRecording(int modelIndex, GLMMDModel* model)
    {
        if (model == nullptr)
        {
            SABA_WARN("ActionRecorder: Cannot start recording with null model.");
            return;
        }

        m_modelIndex = modelIndex;
        m_model = model;
        m_isRecording = true;
        m_lastRecordTime = 0.0;
        m_frames.clear();
        m_boneNames.clear();
        m_lastBoneState.clear();

        MMDModel* mmdModel = model->GetMMDModel();
        if (mmdModel != nullptr)
        {
            auto nodeMan = mmdModel->GetNodeManager();
            if (nodeMan != nullptr)
            {
                size_t nodeCount = nodeMan->GetNodeCount();
                for (size_t i = 0; i < nodeCount; i++)
                {
                    MMDNode* node = nodeMan->GetMMDNode(i);
                    if (node != nullptr)
                    {
                        m_boneNames.push_back(node->GetName());
                    }
                }
            }
        }

        SABA_INFO("ActionRecorder: Started recording model index {} with {} bones.",
                  m_modelIndex, m_boneNames.size());
    }

    void ActionRecorder::StopRecording()
    {
        if (!m_isRecording)
        {
            return;
        }

        m_isRecording = false;
        SABA_INFO("ActionRecorder: Stopped recording. Total frames: {}, Duration: {:.3f}s",
                  m_frames.size(), GetRecordingDuration());
    }

    bool ActionRecorder::IsRecording() const
    {
        return m_isRecording;
    }

    void ActionRecorder::RecordFrame(double time)
    {
        if (!m_isRecording || m_model == nullptr)
        {
            return;
        }

        if (time - m_lastRecordTime < RecordInterval && !m_frames.empty())
        {
            return;
        }

        CaptureCurrentBonePose();
        m_lastRecordTime = time;
    }

    void ActionRecorder::CaptureCurrentBonePose()
    {
        MMDModel* mmdModel = m_model->GetMMDModel();
        if (mmdModel == nullptr)
        {
            return;
        }

        auto nodeMan = mmdModel->GetNodeManager();
        if (nodeMan == nullptr)
        {
            return;
        }

        RecordedFrame frame;
        frame.m_time = m_lastRecordTime;

        size_t nodeCount = nodeMan->GetNodeCount();
        for (size_t i = 0; i < nodeCount; i++)
        {
            MMDNode* node = nodeMan->GetMMDNode(i);
            if (node == nullptr)
            {
                continue;
            }

            const std::string& boneName = node->GetName();
            glm::vec3 t = node->GetAnimationTranslate();
            glm::quat r = node->GetAnimationRotate();

            auto it = m_lastBoneState.find(boneName);
            bool changed = true;
            if (it != m_lastBoneState.end())
            {
                float dt = glm::distance2(it->second.m_translate, t);
                float dq = 1.0f - glm::abs(glm::dot(it->second.m_rotate, r));
                changed = (dt > 0.0001f) || (dq > 0.0001f);
            }

            if (changed)
            {
                RecordedBoneFrame boneFrame;
                boneFrame.m_boneName = boneName;
                boneFrame.m_translate = t;
                boneFrame.m_rotate = r;
                frame.m_boneFrames.push_back(boneFrame);

                BoneState state;
                state.m_translate = t;
                state.m_rotate = r;
                m_lastBoneState[boneName] = state;
            }
        }

        m_frames.push_back(std::move(frame));
    }

    double ActionRecorder::GetRecordingDuration() const
    {
        if (m_frames.empty())
        {
            return 0.0;
        }
        return m_frames.back().m_time - m_frames.front().m_time;
    }

    size_t ActionRecorder::GetRecordedBoneCount() const
    {
        return m_boneNames.size();
    }

    void ActionRecorder::Clear()
    {
        m_modelIndex = -1;
        m_model = nullptr;
        m_isRecording = false;
        m_lastRecordTime = 0.0;
        m_frames.clear();
        m_boneNames.clear();
        m_lastBoneState.clear();
    }

    bool ActionRecorder::SaveRecording(const std::string& filePath) const
    {
        File file;
        if (!file.Create(filePath.c_str()))
        {
            SABA_ERROR("ActionRecorder: Failed to create file '{}'.", filePath);
            return false;
        }

        uint32_t magic = RecordFileMagic;
        uint32_t version = RecordFileVersion;
        uint32_t boneCount = static_cast<uint32_t>(m_boneNames.size());
        uint32_t totalFrames = static_cast<uint32_t>(m_frames.size());

        if (!file.Write(&magic) || !file.Write(&version) ||
            !file.Write(&boneCount) || !file.Write(&totalFrames))
        {
            SABA_ERROR("ActionRecorder: Failed to write header.");
            return false;
        }

        for (const auto& boneName : m_boneNames)
        {
            uint32_t nameLen = static_cast<uint32_t>(boneName.size());
            if (!file.Write(&nameLen))
            {
                return false;
            }
            if (!boneName.empty())
            {
                if (!file.Write(const_cast<char*>(boneName.data()), nameLen))
                {
                    return false;
                }
            }
        }

        for (const auto& frame : m_frames)
        {
            float timeFloat = static_cast<float>(frame.m_time);
            uint32_t changedCount = static_cast<uint32_t>(frame.m_boneFrames.size());

            if (!file.Write(&timeFloat) || !file.Write(&changedCount))
            {
                SABA_ERROR("ActionRecorder: Failed to write frame header.");
                return false;
            }

            for (const auto& boneFrame : frame.m_boneFrames)
            {
                uint32_t boneIndex = static_cast<uint32_t>(-1);
                for (uint32_t bi = 0; bi < m_boneNames.size(); bi++)
                {
                    if (m_boneNames[bi] == boneFrame.m_boneName)
                    {
                        boneIndex = bi;
                        break;
                    }
                }

                if (!file.Write(&boneIndex))
                {
                    return false;
                }

                glm::vec3 t = boneFrame.m_translate;
                glm::quat q = boneFrame.m_rotate;
                if (!file.Write(&t) || !file.Write(&q))
                {
                    return false;
                }
            }
        }

        SABA_INFO("ActionRecorder: Saved recording to '{}' ({} frames, {} bones).",
                  filePath, totalFrames, boneCount);
        return true;
    }

    bool ActionRecorder::LoadRecording(const std::string& filePath)
    {
        File file;
        if (!file.Open(filePath.c_str()))
        {
            SABA_ERROR("ActionRecorder: Failed to open file '{}'.", filePath);
            return false;
        }

        uint32_t magic = 0;
        uint32_t version = 0;
        uint32_t boneCount = 0;
        uint32_t totalFrames = 0;

        if (!file.Read(&magic) || !file.Read(&version) ||
            !file.Read(&boneCount) || !file.Read(&totalFrames))
        {
            SABA_ERROR("ActionRecorder: Failed to read header.");
            return false;
        }

        if (magic != RecordFileMagic)
        {
            SABA_ERROR("ActionRecorder: Invalid file magic. Expected {}, got {}.",
                       RecordFileMagic, magic);
            return false;
        }

        if (version != RecordFileVersion)
        {
            SABA_ERROR("ActionRecorder: Unsupported version {}.", version);
            return false;
        }

        Clear();

        m_boneNames.resize(boneCount);
        for (uint32_t bi = 0; bi < boneCount; bi++)
        {
            uint32_t nameLen = 0;
            if (!file.Read(&nameLen))
            {
                return false;
            }
            m_boneNames[bi].resize(nameLen);
            if (nameLen > 0)
            {
                if (!file.Read(const_cast<char*>(m_boneNames[bi].data()), nameLen))
                {
                    return false;
                }
            }
        }

        m_frames.resize(totalFrames);
        for (uint32_t fi = 0; fi < totalFrames; fi++)
        {
            float timeFloat = 0.0f;
            uint32_t changedCount = 0;

            if (!file.Read(&timeFloat) || !file.Read(&changedCount))
            {
                SABA_ERROR("ActionRecorder: Failed to read frame {} header.", fi);
                return false;
            }

            m_frames[fi].m_time = static_cast<double>(timeFloat);
            m_frames[fi].m_boneFrames.resize(changedCount);

            for (uint32_t bfi = 0; bfi < changedCount; bfi++)
            {
                uint32_t boneIndex = 0;
                if (!file.Read(&boneIndex))
                {
                    return false;
                }

                auto& boneFrame = m_frames[fi].m_boneFrames[bfi];
                if (boneIndex < m_boneNames.size())
                {
                    boneFrame.m_boneName = m_boneNames[boneIndex];
                }

                if (!file.Read(&boneFrame.m_translate) || !file.Read(&boneFrame.m_rotate))
                {
                    return false;
                }
            }
        }

        SABA_INFO("ActionRecorder: Loaded recording from '{}' ({} frames, {} bones).",
                  filePath, totalFrames, boneCount);
        return true;
    }
}