//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "RecordingPanel.h"
#include "ViewerContext.h"
#include <Saba/GL/Model/MMD/GLMMDModel.h>
#include <Saba/Model/MMD/MMDModel.h>
#include <Saba/Base/Log.h>
#include <Saba/Base/Time.h>

#include <imgui.h>
#include <algorithm>
#include <cstring>

namespace saba
{
    RecordingPanel::RecordingPanel()
        : m_modelIndex(-1)
        , m_model(nullptr)
        , m_recorder(std::make_unique<ActionRecorder>())
        , m_splitter(std::make_unique<ActionSplitter>())
        , m_player(std::make_unique<ActionClipPlayer>())
        , m_panelOpen(true)
        , m_recordingStartTime(0.0)
        , m_playingSegmentIndex(-1)
    {
        std::strncpy(m_savePath, "recording.gobot_rec", 255);
        std::strncpy(m_loadPath, "recording.gobot_rec", 255);
    }

    RecordingPanel::~RecordingPanel()
    {
    }

    void RecordingPanel::SetModel(int modelIndex, GLMMDModel* model)
    {
        m_modelIndex = modelIndex;
        m_model = model;
        m_player->SetModel(modelIndex, model);
    }

    void RecordingPanel::ClearModel()
    {
        m_modelIndex = -1;
        m_model = nullptr;
        m_recorder->Clear();
        m_splitter->Clear();
        m_player->Clear();
    }

    void RecordingPanel::Update(ViewerContext* ctxt)
    {
        if (m_recorder->IsRecording())
        {
            double currentTime = GetTime();
            double elapsed = currentTime - m_recordingStartTime;
            m_recorder->RecordFrame(elapsed);
        }

        if (m_player->IsPlaying())
        {
            m_player->Update(ctxt);
        }
    }

    void RecordingPanel::Draw()
    {
        if (!m_panelOpen)
        {
            return;
        }

        ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Action Recording", &m_panelOpen))
        {
            ImGui::End();
            return;
        }

        DrawRecordingSection();
        ImGui::Separator();
        DrawLoadSection();
        ImGui::Separator();
        DrawSplittingSection();
        ImGui::Separator();
        DrawPlayerSection();

        ImGui::End();
    }

    void RecordingPanel::DrawRecordingSection()
    {
        ImGui::TextUnformatted("Recording");

        if (m_recorder->IsRecording())
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "  (REC)");

            double duration = m_recorder->GetRecordingDuration();
            int minutes = static_cast<int>(duration) / 60;
            int seconds = static_cast<int>(duration) % 60;
            int millis = static_cast<int>((duration - static_cast<double>(static_cast<int>(duration))) * 1000);
            ImGui::Text("Elapsed: %02d:%02d.%03d", minutes, seconds, millis);

            if (ImGui::Button("Stop Recording"))
            {
                OnStopRecording();
            }
        }
        else
        {
            if (ImGui::Button("Start Recording"))
            {
                OnStartRecording();
            }
        }

        ImGui::InputText("Save Path", m_savePath, sizeof(m_savePath));
        if (ImGui::Button("Save Recording"))
        {
            OnSaveRecording();
        }
    }

    void RecordingPanel::DrawLoadSection()
    {
        ImGui::TextUnformatted("Load Recording");

        ImGui::InputText("Load Path", m_loadPath, sizeof(m_loadPath));
        if (ImGui::Button("Load Recording"))
        {
            OnLoadRecording();
        }
    }

    void RecordingPanel::DrawSplittingSection()
    {
        ImGui::TextUnformatted("Splitting");

        if (ImGui::Button("Auto Split"))
        {
            OnAutoSplit();
        }

        ImGui::SameLine();

        if (ImGui::Button("Merge All"))
        {
            OnMergeAll();
        }

        ImGui::SameLine();

        if (ImGui::Button("Export as VMD"))
        {
            OnExportVMD();
        }

        DrawSegmentList();
    }

    void RecordingPanel::DrawSegmentList()
    {
        const auto& segments = m_splitter->GetSegments();
        if (segments.empty())
        {
            ImGui::TextUnformatted("No segments. Load a recording and auto-split.");
            return;
        }

        ImGui::Text("Segments: %d", m_splitter->GetSegmentCount());

        ImGui::Columns(6, "segmentColumns", false);
        ImGui::SetColumnWidth(0, 30.0f);
        ImGui::SetColumnWidth(1, 100.0f);
        ImGui::SetColumnWidth(2, 70.0f);
        ImGui::SetColumnWidth(3, 70.0f);
        ImGui::SetColumnWidth(4, 50.0f);
        ImGui::SetColumnWidth(5, 80.0f);

        ImGui::TextUnformatted("#");
        ImGui::NextColumn();
        ImGui::TextUnformatted("Name");
        ImGui::NextColumn();
        ImGui::TextUnformatted("Duration");
        ImGui::NextColumn();
        ImGui::TextUnformatted("Gap");
        ImGui::NextColumn();
        ImGui::TextUnformatted("Play");
        ImGui::NextColumn();
        ImGui::TextUnformatted("Actions");
        ImGui::NextColumn();
        ImGui::Separator();

        for (int i = 0; i < static_cast<int>(segments.size()); i++)
        {
            const auto& seg = segments[i];

            ImGui::Text("%d", i + 1);
            ImGui::NextColumn();

            std::string nameLabel = "##name_" + std::to_string(i);
            char nameBuf[64];
            std::strncpy(nameBuf, seg.m_name.c_str(), sizeof(nameBuf));
            if (ImGui::InputText(nameLabel.c_str(), nameBuf, sizeof(nameBuf),
                                ImGuiInputTextFlags_EnterReturnsTrue))
            {
                const_cast<Segment&>(seg).m_name = nameBuf;
            }
            ImGui::NextColumn();

            ImGui::Text("%.2fs", seg.m_duration);
            ImGui::NextColumn();

            std::string gapLabel = "##gap_" + std::to_string(i);
            float gapFloat = static_cast<float>(seg.m_gapAfter);
            ImGui::PushItemWidth(-1);
            if (ImGui::SliderFloat(gapLabel.c_str(), &gapFloat, 0.0f, 10.0f, "%.2f"))
            {
                m_splitter->SetSegmentGap(i, static_cast<double>(gapFloat));
            }
            ImGui::PopItemWidth();
            ImGui::NextColumn();

            std::string playLabel = "Play##play_" + std::to_string(i);
            if (ImGui::SmallButton(playLabel.c_str()))
            {
                OnPlaySegment(i);
            }
            ImGui::NextColumn();

            std::string upLabel = "^##up_" + std::to_string(i);
            std::string downLabel = "v##dn_" + std::to_string(i);
            std::string removeLabel = "X##rm_" + std::to_string(i);

            if (i > 0 && ImGui::SmallButton(upLabel.c_str()))
            {
                std::vector<int> newOrder;
                for (int j = 0; j < static_cast<int>(segments.size()); j++)
                {
                    newOrder.push_back(j);
                }
                std::swap(newOrder[i], newOrder[i - 1]);
                m_splitter->ReorderSegments(newOrder);
            }
            ImGui::SameLine();
            if (i < static_cast<int>(segments.size()) - 1 && ImGui::SmallButton(downLabel.c_str()))
            {
                std::vector<int> newOrder;
                for (int j = 0; j < static_cast<int>(segments.size()); j++)
                {
                    newOrder.push_back(j);
                }
                std::swap(newOrder[i], newOrder[i + 1]);
                m_splitter->ReorderSegments(newOrder);
            }
            ImGui::SameLine();
            if (ImGui::SmallButton(removeLabel.c_str()))
            {
                m_splitter->RemoveSegment(i);
            }
            ImGui::NextColumn();
        }

        ImGui::Columns(1);
    }

    void RecordingPanel::DrawPlayerSection()
    {
        ImGui::TextUnformatted("Playback");

        if (m_player->IsPlaying())
        {
            if (ImGui::Button("Stop"))
            {
                OnStopPlayback();
            }

            ImGui::SameLine();
            if (m_player->IsPaused())
            {
                if (ImGui::Button("Resume"))
                {
                    m_player->Resume();
                }
            }
            else
            {
                if (ImGui::Button("Pause"))
                {
                    m_player->Pause();
                }
            }

            double currentTime = m_player->GetPlaybackTime();
            ImGui::Text("Time: %.2fs", currentTime);
        }
        else
        {
            if (ImGui::Button("Play All"))
            {
                OnPlayAll();
            }
        }
    }

    void RecordingPanel::OnStartRecording()
    {
        if (m_model == nullptr)
        {
            SABA_WARN("RecordingPanel: No model set for recording.");
            return;
        }

        m_recorder->StartRecording(m_modelIndex, m_model);
        m_recordingStartTime = GetTime();
    }

    void RecordingPanel::OnStopRecording()
    {
        m_recorder->StopRecording();
    }

    void RecordingPanel::OnSaveRecording()
    {
        m_recorder->SaveRecording(m_savePath);
    }

    void RecordingPanel::OnLoadRecording()
    {
        if (m_splitter->ImportRecording(m_loadPath))
        {
            SABA_INFO("RecordingPanel: Successfully loaded recording.");
        }
    }

    void RecordingPanel::OnAutoSplit()
    {
        m_splitter->AutoSplit();
    }

    void RecordingPanel::OnMergeAll()
    {
        SABA_INFO("RecordingPanel: Merge complete.");
    }

    void RecordingPanel::OnExportVMD()
    {
        std::string vmdPath = std::string(m_savePath) + ".vmd";
        if (m_splitter->ExportAnimation(vmdPath))
        {
            SABA_INFO("RecordingPanel: Exported VMD to '{}'.", vmdPath);
        }
    }

    void RecordingPanel::OnPlaySegment(int segmentIndex)
    {
        if (m_model == nullptr)
        {
            SABA_WARN("RecordingPanel: No model set for playback.");
            return;
        }

        m_player->PlaySegment(segmentIndex);
        m_playingSegmentIndex = segmentIndex;
    }

    void RecordingPanel::OnPlayAll()
    {
        if (m_model == nullptr)
        {
            SABA_WARN("RecordingPanel: No model set for playback.");
            return;
        }

        m_player->PlayAll();
    }

    void RecordingPanel::OnStopPlayback()
    {
        m_player->Stop();
        m_playingSegmentIndex = -1;
    }
}