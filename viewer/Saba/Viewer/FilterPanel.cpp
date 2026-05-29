//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "FilterPanel.h"
#include "ViewerContext.h"

#include <imgui.h>

#include <string>
#include <array>

namespace saba
{
    FilterPanel::FilterPanel()
        : m_context(nullptr)
        , m_filter(nullptr)
        , m_enabled(false)
        , m_paramsChanged(false)
        , m_selectedFilterType(FilterType::None)
    {
    }

    FilterPanel::~FilterPanel()
    {
    }

    void FilterPanel::SetViewerContext(ViewerContext* context)
    {
        m_context = context;
    }

    void FilterPanel::SetGLFilter(GLFilter* filter)
    {
        m_filter = filter;
    }

    void FilterPanel::Draw()
    {
        if (!m_enabled || m_filter == nullptr)
        {
            return;
        }

        ImGui::SetNextWindowSize(ImVec2(320, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("Filter", &m_enabled);

        ImGui::PushID("FilterPanel");

        const char* filterNames[] = {
            "None",
            "Bright White",
            "Skin Bright",
            "Dark",
            "Genshin Day",
            "Genshin Night",
            "Genshin Dusk",
            "Custom"
        };

        int currentFilter = static_cast<int>(m_selectedFilterType);
        if (ImGui::Combo("Preset", &currentFilter, filterNames, IM_ARRAYSIZE(filterNames)))
        {
            m_selectedFilterType = static_cast<FilterType>(currentFilter);
            m_paramsChanged = true;

            if (m_selectedFilterType != FilterType::Custom)
            {
                m_editParams = GLFilter::GetPresetParameters(m_selectedFilterType);
            }
        }

        float intensity = m_filter->GetIntensity();
        if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 1.0f))
        {
            m_filter->SetIntensity(intensity);
        }

        ImGui::Separator();
        ImGui::Text("Parameters");

        if (m_selectedFilterType == FilterType::Custom ||
            m_selectedFilterType == FilterType::BrightWhite ||
            m_selectedFilterType == FilterType::SkinBright ||
            m_selectedFilterType == FilterType::Dark ||
            m_selectedFilterType == FilterType::GenshinDay ||
            m_selectedFilterType == FilterType::GenshinNight ||
            m_selectedFilterType == FilterType::GenshinDusk)
        {
            bool paramChanged = false;

            paramChanged |= ImGui::SliderFloat("Exposure", &m_editParams.m_exposure, -2.0f, 4.0f);
            paramChanged |= ImGui::SliderFloat("Contrast", &m_editParams.m_contrast, 0.0f, 3.0f);
            paramChanged |= ImGui::SliderFloat("Saturation", &m_editParams.m_saturation, 0.0f, 3.0f);
            paramChanged |= ImGui::SliderFloat("Hue Shift", &m_editParams.m_hueShift, -180.0f, 180.0f);
            paramChanged |= ImGui::SliderFloat("Temperature", &m_editParams.m_temperature, -100.0f, 100.0f);
            paramChanged |= ImGui::SliderFloat("Tint", &m_editParams.m_tint, -100.0f, 100.0f);

            if (paramChanged)
            {
                m_paramsChanged = true;
                if (m_selectedFilterType == FilterType::Custom)
                {
                    m_filter->SetCustomParameters(m_editParams);
                }
            }
        }

        ImGui::Separator();

        if (ImGui::Button("Reset to Defaults"))
        {
            m_editParams = FilterParameters();
            m_selectedFilterType = FilterType::None;
            m_filter->SetIntensity(1.0f);
            m_filter->SetCustomParameters(FilterParameters());
            m_paramsChanged = true;
        }

        if (m_paramsChanged)
        {
            if (m_selectedFilterType == FilterType::Custom)
            {
                m_filter->SetCustomParameters(m_editParams);
                m_filter->Apply(FilterType::Custom);
            }
            else
            {
                m_filter->Apply(m_selectedFilterType);
            }
            m_paramsChanged = false;
        }

        ImGui::PopID();
        ImGui::End();
    }
}