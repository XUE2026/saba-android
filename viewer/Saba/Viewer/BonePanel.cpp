//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "BonePanel.h"
#include "../GL/Model/MMD/GLMMDModel.h"
#include <Saba/Model/MMD/MMDModel.h>
#include <Saba/Model/MMD/MMDNode.h>
#include <imgui.h>

namespace saba
{
    BonePanel::BonePanel()
        : m_modelIndex(-1)
        , m_model(nullptr)
        , m_bonePanelOpen(true)
        , m_animPanelOpen(true)
        , m_selectedBoneIndex(-1)
        , m_rotX(0.0f)
        , m_rotY(0.0f)
        , m_rotZ(0.0f)
        , m_transX(0.0f)
        , m_transY(0.0f)
        , m_transZ(0.0f)
        , m_animTime(0.0f)
        , m_currentAnimItem(0)
    {
        m_boneManipulator = std::make_unique<BoneManipulator>();
        m_animationGenerator = std::make_unique<AnimationGenerator>();
    }

    void BonePanel::SetModel(int modelIndex, GLMMDModel* model)
    {
        m_modelIndex = modelIndex;
        m_model = model;
        m_selectedBoneIndex = -1;
        m_rotX = m_rotY = m_rotZ = 0.0f;
        m_transX = m_transY = m_transZ = 0.0f;
    }

    void BonePanel::ClearModel()
    {
        m_modelIndex = -1;
        m_model = nullptr;
        m_boneManipulator->ResetAllBones();
        m_animationGenerator->StopBuiltinAnimation();
    }

    void BonePanel::Draw()
    {
        DrawBonePanel();

        ImGui::SameLine();

        DrawAnimPanel();
    }

    void BonePanel::DrawBonePanel()
    {
        if (!m_bonePanelOpen)
        {
            return;
        }

        ImGui::Begin("Bone Manipulator", &m_bonePanelOpen);

        if (m_model == nullptr)
        {
            ImGui::Text("No model selected.");
            ImGui::End();
            return;
        }

        MMDModel* mmdModel = m_model->GetMMDModel();
        if (mmdModel == nullptr)
        {
            ImGui::Text("Invalid model.");
            ImGui::End();
            return;
        }

        auto nodeMan = mmdModel->GetNodeManager();
        if (nodeMan == nullptr)
        {
            ImGui::Text("No node manager.");
            ImGui::End();
            return;
        }

        std::vector<std::string> boneList = m_boneManipulator->GetBoneList(mmdModel);

        std::vector<const char*> boneCStr;
        boneCStr.reserve(boneList.size());
        for (const auto& name : boneList)
        {
            boneCStr.push_back(name.c_str());
        }

        if (m_selectedBoneIndex >= (int)boneList.size())
        {
            m_selectedBoneIndex = -1;
        }

        ImGui::Text("Select Bone");
        ImGui::PushItemWidth(200.0f);
        if (ImGui::Combo("##bone_selector", &m_selectedBoneIndex, boneCStr.data(), (int)boneCStr.size()))
        {
            if (m_selectedBoneIndex >= 0 && m_selectedBoneIndex < (int)boneList.size())
            {
                m_boneManipulator->SelectBone(m_modelIndex, boneList[m_selectedBoneIndex]);

                size_t nodeIdx = nodeMan->FindNodeIndex(boneList[m_selectedBoneIndex]);
                if (nodeIdx != MMDNodeManager::NPos)
                {
                    auto node = nodeMan->GetMMDNode(nodeIdx);
                    if (node != nullptr)
                    {
                        m_rotX = m_rotY = m_rotZ = 0.0f;
                        m_transX = m_transY = m_transZ = 0.0f;
                    }
                }
            }
        }
        ImGui::PopItemWidth();

        if (m_selectedBoneIndex >= 0)
        {
            ImGui::Separator();
            ImGui::Text("Bone: %s", boneCStr[m_selectedBoneIndex]);

            ImGui::Text("Rotation (degrees)");
            ImGui::PushItemWidth(150.0f);
            ImGui::SliderFloat("X", &m_rotX, -180.0f, 180.0f);
            ImGui::SliderFloat("Y", &m_rotY, -180.0f, 180.0f);
            ImGui::SliderFloat("Z", &m_rotZ, -180.0f, 180.0f);
            ImGui::PopItemWidth();

            ImGui::Text("Translation");
            ImGui::PushItemWidth(150.0f);
            ImGui::SliderFloat("Tx", &m_transX, -10.0f, 10.0f);
            ImGui::SliderFloat("Ty", &m_transY, -10.0f, 10.0f);
            ImGui::SliderFloat("Tz", &m_transZ, -10.0f, 10.0f);
            ImGui::PopItemWidth();

            if (ImGui::Button("Apply##bone"))
            {
                m_boneManipulator->RotateBone(m_rotX, m_rotY, m_rotZ);
                m_boneManipulator->TranslateBone(m_transX, m_transY, m_transZ);
            }

            ImGui::SameLine();

            if (ImGui::Button("Reset Bone"))
            {
                m_boneManipulator->ResetBone();
                m_rotX = m_rotY = m_rotZ = 0.0f;
                m_transX = m_transY = m_transZ = 0.0f;
            }

            ImGui::SameLine();

            if (ImGui::Button("Reset All"))
            {
                m_boneManipulator->ResetAllBones();
                m_rotX = m_rotY = m_rotZ = 0.0f;
                m_transX = m_transY = m_transZ = 0.0f;
            }
        }
        else
        {
            ImGui::Separator();
            if (ImGui::Button("Reset All##bone"))
            {
                m_boneManipulator->ResetAllBones();
                m_rotX = m_rotY = m_rotZ = 0.0f;
                m_transX = m_transY = m_transZ = 0.0f;
            }
        }

        if (m_boneManipulator->HasOverrides())
        {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Modified Bones: %zu",
                               m_boneManipulator->GetOverrides().size());
        }

        ImGui::End();
    }

    void BonePanel::DrawAnimPanel()
    {
        if (!m_animPanelOpen)
        {
            return;
        }

        ImGui::Begin("Built-in Animation", &m_animPanelOpen);

        if (m_model == nullptr)
        {
            ImGui::Text("No model selected.");
            ImGui::End();
            return;
        }

        bool isPlaying = m_animationGenerator->IsPlaying();
        BuiltinAnimType currentType = m_animationGenerator->GetCurrentAnimType();

        if (isPlaying)
        {
            const char* animNames[] = { "None", "Idle", "Walk", "Run", "Jump" };
            int typeIdx = (int)currentType;
            if (typeIdx >= 0 && typeIdx < 5)
            {
                ImGui::Text("Current Animation: %s", animNames[typeIdx]);
            }

            float duration = m_animationGenerator->GetDuration();
            float progress = 0.0f;
            if (duration > 0.0f)
            {
                progress = fmod(m_animTime, duration) / duration;
            }
            ImGui::ProgressBar(progress, ImVec2(-1.0f, 0.0f));
        }

        ImGui::Separator();
        ImGui::Text("Play Animation:");

        if (ImGui::Button("Idle", ImVec2(80, 0)))
        {
            m_animationGenerator->PlayBuiltinAnimation(BuiltinAnimType::Idle, m_modelIndex);
            m_animTime = 0.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("Walk", ImVec2(80, 0)))
        {
            m_animationGenerator->PlayBuiltinAnimation(BuiltinAnimType::Walk, m_modelIndex);
            m_animTime = 0.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("Run", ImVec2(80, 0)))
        {
            m_animationGenerator->PlayBuiltinAnimation(BuiltinAnimType::Run, m_modelIndex);
            m_animTime = 0.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("Jump", ImVec2(80, 0)))
        {
            m_animationGenerator->PlayBuiltinAnimation(BuiltinAnimType::Jump, m_modelIndex);
            m_animTime = 0.0f;
        }

        if (isPlaying)
        {
            ImGui::Separator();
            if (ImGui::Button("Stop Animation", ImVec2(160, 0)))
            {
                m_animationGenerator->StopBuiltinAnimation();
                m_animTime = 0.0f;
            }
        }

        if (m_boneManipulator->HasOverrides())
        {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.8f, 0.6f, 0.2f, 1.0f),
                               "Bone overrides active - animation may be affected.");
        }

        ImGui::End();
    }

    void BonePanel::ApplyBonePoseToModel()
    {
        if (m_model == nullptr)
        {
            return;
        }

        MMDModel* mmdModel = m_model->GetMMDModel();
        if (mmdModel == nullptr)
        {
            return;
        }

        m_boneManipulator->Apply(mmdModel, 1.0f);
    }

    void BonePanel::ApplyAnimationToModel(float time, float deltaTime)
    {
        if (m_model == nullptr || !m_animationGenerator->IsPlaying())
        {
            return;
        }

        MMDModel* mmdModel = m_model->GetMMDModel();
        if (mmdModel == nullptr)
        {
            return;
        }

        m_animTime += deltaTime;
        m_animationGenerator->Generate(m_animTime, mmdModel, 1.0f);
    }
}