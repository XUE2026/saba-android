//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "EnvironmentPanel.h"

#include <imgui.h>

#include <cstring>

namespace saba
{
    static const char* g_primitiveTypeNames[] = {
        "Cube",
        "Sphere",
        "Cone",
        "Cylinder"
    };

    static const char* g_groundTypeNames[] = {
        "Floor",
        "Grass",
        "Mud",
        "Asphalt",
        "Sky",
        "Field"
    };

    static const char* g_envObjectTypeNames[] = {
        "Tree",
        "Flower",
        "Bird",
        "Butterfly"
    };

    EnvironmentPanel::EnvironmentPanel()
        : m_envManager(nullptr)
        , m_selectedPrimitiveType(0)
        , m_selectedGroundType(0)
        , m_selectedEnvObjectType(0)
        , m_enablePanel(true)
    {
        m_primitiveSize[0] = 1.0f;
        m_primitiveSize[1] = 1.0f;
        m_primitiveSize[2] = 1.0f;
        m_primitivePosition[0] = 0.0f;
        m_primitivePosition[1] = 0.0f;
        m_primitivePosition[2] = 0.0f;
        m_primitiveColor[0] = 1.0f;
        m_primitiveColor[1] = 1.0f;
        m_primitiveColor[2] = 1.0f;
        m_primitiveColor[3] = 1.0f;

        m_envObjectPosition[0] = 0.0f;
        m_envObjectPosition[1] = 1.0f;
        m_envObjectPosition[2] = 0.0f;
        m_envObjectSize[0] = 1.0f;
        m_envObjectSize[1] = 1.0f;
        m_envObjectSize[2] = 1.0f;
        m_envObjectColor[0] = 0.0f;
        m_envObjectColor[1] = 0.8f;
        m_envObjectColor[2] = 0.0f;
        m_envObjectColor[3] = 1.0f;
    }

    void EnvironmentPanel::SetEnvironmentManager(EnvironmentManager* envManager)
    {
        m_envManager = envManager;
    }

    void EnvironmentPanel::Draw()
    {
        if (!m_enablePanel || m_envManager == nullptr)
        {
            return;
        }

        ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Environment", &m_enablePanel))
        {
            ImGui::End();
            return;
        }

        if (ImGui::CollapsingHeader("Sun Light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            DrawSunControl();
        }

        if (ImGui::CollapsingHeader("Ground", ImGuiTreeNodeFlags_DefaultOpen))
        {
            DrawGroundControl();
        }

        if (ImGui::CollapsingHeader("Add Primitive", ImGuiTreeNodeFlags_DefaultOpen))
        {
            DrawPrimitiveControl();
        }

        if (ImGui::CollapsingHeader("Primitive List"))
        {
            DrawPrimitiveList();
        }

        if (ImGui::CollapsingHeader("Environment Objects"))
        {
            DrawEnvObjectControl();
        }

        ImGui::End();
    }

    void EnvironmentPanel::DrawSunControl()
    {
        ImGui::PushID("SunControl");

        glm::vec3 dir = m_envManager->GetSunLight().m_direction;
        float dirArr[3] = { dir.x, dir.y, dir.z };
        if (ImGui::SliderFloat3("Direction", dirArr, -1.0f, 1.0f))
        {
            m_envManager->SetSunDirection(dirArr[0], dirArr[1], dirArr[2]);
        }

        float intensity = m_envManager->GetSunLight().m_intensity;
        if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f))
        {
            m_envManager->SetSunIntensity(intensity);
        }

        glm::vec3 color = m_envManager->GetSunLight().m_color;
        float colorArr[3] = { color.r, color.g, color.b };
        if (ImGui::ColorEdit3("Color", colorArr))
        {
            m_envManager->SetSunColor(colorArr[0], colorArr[1], colorArr[2]);
        }

        ImGui::PopID();
    }

    void EnvironmentPanel::DrawGroundControl()
    {
        ImGui::PushID("GroundControl");

        int currentType = static_cast<int>(m_envManager->GetGround().m_type);
        if (ImGui::Combo("Type", &currentType, g_groundTypeNames, IM_ARRAYSIZE(g_groundTypeNames)))
        {
            m_envManager->SetGroundType(static_cast<EnvironmentManager::GroundType>(currentType));
        }

        ImGui::PopID();
    }

    void EnvironmentPanel::DrawPrimitiveControl()
    {
        ImGui::PushID("PrimitiveControl");

        ImGui::Combo("Type", &m_selectedPrimitiveType, g_primitiveTypeNames, IM_ARRAYSIZE(g_primitiveTypeNames));
        ImGui::SliderFloat3("Size", m_primitiveSize, 0.1f, 10.0f);
        ImGui::SliderFloat3("Position", m_primitivePosition, -20.0f, 20.0f);
        ImGui::ColorEdit4("Color", m_primitiveColor);

        if (ImGui::Button("Add Primitive", ImVec2(120, 0)))
        {
            auto type = static_cast<EnvironmentManager::PrimitiveType>(m_selectedPrimitiveType);
            glm::vec3 size(m_primitiveSize[0], m_primitiveSize[1], m_primitiveSize[2]);
            glm::vec3 pos(m_primitivePosition[0], m_primitivePosition[1], m_primitivePosition[2]);
            glm::vec4 color(m_primitiveColor[0], m_primitiveColor[1], m_primitiveColor[2], m_primitiveColor[3]);
            m_envManager->AddPrimitive(type, size, pos, color);
        }

        ImGui::SameLine();
        if (ImGui::Button("Clear All", ImVec2(80, 0)))
        {
            m_envManager->ClearPrimitives();
        }

        ImGui::PopID();
    }

    void EnvironmentPanel::DrawPrimitiveList()
    {
        ImGui::PushID("PrimitiveList");

        const auto& primitives = m_envManager->GetPrimitives();
        if (primitives.empty())
        {
            ImGui::Text("No primitives added.");
        }
        else
        {
            for (size_t i = 0; i < primitives.size(); i++)
            {
                ImGui::PushID(static_cast<int>(i));

                const auto& prim = primitives[i];
                const char* typeName = g_primitiveTypeNames[static_cast<int>(prim.m_type)];
                ImGui::Text("[%zu] %s", i, typeName);
                ImGui::SameLine();
                ImGui::TextDisabled("(%.1f, %.1f, %.1f)",
                    prim.m_position.x, prim.m_position.y, prim.m_position.z);

                if (ImGui::SmallButton("Delete"))
                {
                    m_envManager->RemovePrimitive(i);
                    ImGui::PopID();
                    break;
                }

                ImGui::PopID();
            }
        }

        ImGui::PopID();
    }

    void EnvironmentPanel::DrawEnvObjectControl()
    {
        ImGui::PushID("EnvObjectControl");

        ImGui::Combo("Type", &m_selectedEnvObjectType, g_envObjectTypeNames, IM_ARRAYSIZE(g_envObjectTypeNames));
        ImGui::SliderFloat3("Position", m_envObjectPosition, -20.0f, 20.0f);
        ImGui::SliderFloat3("Size", m_envObjectSize, 0.1f, 10.0f);
        ImGui::ColorEdit4("Color", m_envObjectColor);

        if (ImGui::Button("Add Object", ImVec2(120, 0)))
        {
            auto type = static_cast<EnvironmentManager::EnvObjectType>(m_selectedEnvObjectType);
            glm::vec3 pos(m_envObjectPosition[0], m_envObjectPosition[1], m_envObjectPosition[2]);
            glm::vec3 size(m_envObjectSize[0], m_envObjectSize[1], m_envObjectSize[2]);
            glm::vec4 color(m_envObjectColor[0], m_envObjectColor[1], m_envObjectColor[2], m_envObjectColor[3]);
            m_envManager->AddEnvironmentObject(type, pos, size, color);
        }

        ImGui::SameLine();
        if (ImGui::Button("Clear##Env", ImVec2(80, 0)))
        {
            m_envManager->ClearEnvironmentObjects();
        }

        ImGui::Separator();
        const auto& envObjects = m_envManager->GetEnvObjects();
        if (!envObjects.empty())
        {
            ImGui::Text("Objects: %zu", envObjects.size());
            for (size_t i = 0; i < envObjects.size(); i++)
            {
                ImGui::PushID(static_cast<int>(i + 1000));
                const auto& obj = envObjects[i];
                const char* typeName = g_envObjectTypeNames[static_cast<int>(obj.m_type)];
                ImGui::Text("[%zu] %s", i, typeName);
                ImGui::SameLine();
                if (ImGui::SmallButton("Delete"))
                {
                    m_envManager->RemoveEnvironmentObject(i);
                    ImGui::PopID();
                    break;
                }
                ImGui::PopID();
            }
        }

        ImGui::PopID();
    }
}