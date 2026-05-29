//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "OptimizationPanel.h"
#include "ModelPreloader.h"

#include <Saba/Base/Log.h>
#include <Saba/Model/MMD/ModelCache.h>
#include <Saba/GL/GLTextureCache.h>

#include <imgui.h>

#include <cstring>

namespace saba
{
	OptimizationPanel::OptimizationPanel()
		: m_visible(false)
		, m_maxModels(5)
		, m_maxTextures(50)
		, m_optimizeForMobile(false)
	{
	}

	OptimizationPanel::~OptimizationPanel()
	{
	}

	void OptimizationPanel::SetModelPreloader(std::shared_ptr<ModelPreloader> preloader)
	{
		m_preloader = preloader;
	}

	void OptimizationPanel::Draw()
	{
		if (!m_visible)
		{
			return;
		}

		ImGui::SetNextWindowSize(ImVec2(380, 500), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Optimization Panel", &m_visible))
		{
			ImGui::End();
			return;
		}

		if (ImGui::CollapsingHeader("Cache Settings", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawCacheSettings();
		}

		if (ImGui::CollapsingHeader("Model Preloader", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawPreloaderSection();
		}

		if (ImGui::CollapsingHeader("Memory Usage", ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawMemoryInfo();
		}

		if (ImGui::CollapsingHeader("Mobile Optimization"))
		{
			DrawMobileOptimization();
		}

		ImGui::End();
	}

	void OptimizationPanel::DrawCacheSettings()
	{
		ImGui::Text("Configure model and texture caching behavior.");
		ImGui::Separator();

		int maxModels = m_maxModels;
		if (ImGui::SliderInt("Max Cached Models", &maxModels, 1, 20))
		{
			m_maxModels = maxModels;
			ModelCache::Get().SetMaxModels(static_cast<size_t>(m_maxModels));
			SABA_INFO("OptimizationPanel: Max models set to [{}]", m_maxModels);
		}
		ImGui::SameLine();
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Maximum number of models to keep in memory. LRU eviction used when exceeded.");
		}

		int maxTextures = m_maxTextures;
		if (ImGui::SliderInt("Max Cached Textures", &maxTextures, 10, 200))
		{
			m_maxTextures = maxTextures;
			GLTextureCache::Get().SetMaxTextures(static_cast<size_t>(m_maxTextures));
			SABA_INFO("OptimizationPanel: Max textures set to [{}]", m_maxTextures);
		}
		ImGui::SameLine();
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Maximum number of textures to keep in GPU memory.");
		}

		if (ImGui::Button("Clear Model Cache", ImVec2(160, 0)))
		{
			ModelCache::Get().Clear();
			SABA_INFO("OptimizationPanel: Model cache cleared by user");
		}

		ImGui::SameLine();

		if (ImGui::Button("Clear Texture Cache", ImVec2(160, 0)))
		{
			GLTextureCache::Get().Clear();
			SABA_INFO("OptimizationPanel: Texture cache cleared by user");
		}
	}

	void OptimizationPanel::DrawPreloaderSection()
	{
		ImGui::Text("Preload models and textures in the background.");
		ImGui::Separator();

		char pathBuf[512];
		std::strncpy(pathBuf, m_preloadPath.c_str(), sizeof(pathBuf) - 1);
		pathBuf[sizeof(pathBuf) - 1] = '\0';

		ImGui::InputText("Model Path", pathBuf, sizeof(pathBuf));
		m_preloadPath = pathBuf;

		if (ImGui::Button("Preload Model", ImVec2(160, 0)))
		{
			if (!m_preloadPath.empty() && m_preloader)
			{
				SABA_INFO("OptimizationPanel: Starting preload of [{}]", m_preloadPath);
				m_preloader->PreloadModel(m_preloadPath);
			}
		}

		ImGui::SameLine();

		if (m_preloader && !m_preloader->IsPreloadComplete())
		{
			if (ImGui::Button("Cancel", ImVec2(80, 0)))
			{
				m_preloader->CancelPreload();
				SABA_INFO("OptimizationPanel: Preload cancelled by user");
			}
		}

		if (m_preloader)
		{
			float progress = m_preloader->GetPreloadProgress();
			char progressLabel[64];
			std::snprintf(progressLabel, sizeof(progressLabel), "%.0f%%", progress * 100.0f);

			ImGui::ProgressBar(progress, ImVec2(-1, 0), progressLabel);

			if (m_preloader->IsPreloadComplete() && progress >= 1.0f)
			{
				ImGui::TextColored(ImVec4(0, 1, 0, 1), "Preload complete!");
			}
			else if (!m_preloader->IsPreloadComplete())
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Preloading in progress...");
			}
		}
		else
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "No preloader set.");
		}
	}

	void OptimizationPanel::DrawMemoryInfo()
	{
		ImGui::Text("Current cache memory estimates.");
		ImGui::Separator();

		size_t modelCount = ModelCache::Get().GetModelCount();
		size_t maxModels = ModelCache::Get().GetMaxModels();
		ImGui::Text("Cached Models:  %zu / %zu", modelCount, maxModels);

		size_t texCount = GLTextureCache::Get().GetTextureCount();
		size_t maxTextures = GLTextureCache::Get().GetMaxTextures();
		ImGui::Text("Cached Textures: %zu / %zu", texCount, maxTextures);

		size_t texMemBytes = GLTextureCache::Get().GetEstimatedMemoryUsage();
		float texMemMB = static_cast<float>(texMemBytes) / (1024.0f * 1024.0f);
		ImGui::Text("Texture Memory:  ~%.1f MB", texMemMB);

		size_t estimatedModelMemMB = modelCount * 50;
		float totalMB = texMemMB + static_cast<float>(estimatedModelMemMB);
		ImGui::Text("Estimated Total: ~%.1f MB", totalMB);

		ImGui::Separator();
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), "Note: Model memory is estimated at ~50 MB each.");
	}

	void OptimizationPanel::DrawMobileOptimization()
	{
		ImGui::Text("Settings for mobile/Android devices.");
		ImGui::Separator();

		if (ImGui::Checkbox("Optimize for Mobile", &m_optimizeForMobile))
		{
			if (m_optimizeForMobile)
			{
				ModelCache::Get().SetMaxModels(3);
				GLTextureCache::Get().SetMaxTextures(20);
				m_maxModels = 3;
				m_maxTextures = 20;
				SABA_INFO("OptimizationPanel: Mobile optimization enabled (models: 3, textures: 20)");
			}
			else
			{
				ModelCache::Get().SetMaxModels(5);
				GLTextureCache::Get().SetMaxTextures(50);
				m_maxModels = 5;
				m_maxTextures = 50;
				SABA_INFO("OptimizationPanel: Mobile optimization disabled");
			}
		}
		ImGui::SameLine();
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Reduces cache sizes and enables texture compression for mobile devices.");
		}

		if (m_optimizeForMobile)
		{
			ImGui::TextColored(ImVec4(0, 1, 0, 1), "Mobile optimizations active:");
			ImGui::BulletText("Max models reduced to 3");
			ImGui::BulletText("Max textures reduced to 20");
			ImGui::BulletText("ETC2 texture compression preferred");
		}
	}
}