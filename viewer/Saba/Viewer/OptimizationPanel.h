//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_OPTIMIZATIONPANEL_H_
#define SABA_VIEWER_OPTIMIZATIONPANEL_H_

#include <string>
#include <memory>

namespace saba
{
	class ModelPreloader;

	class OptimizationPanel
	{
	public:
		OptimizationPanel();
		~OptimizationPanel();

		OptimizationPanel(const OptimizationPanel&) = delete;
		OptimizationPanel& operator=(const OptimizationPanel&) = delete;

		void SetModelPreloader(std::shared_ptr<ModelPreloader> preloader);

		void Draw();

		bool IsVisible() const { return m_visible; }
		void SetVisible(bool visible) { m_visible = visible; }
		void ToggleVisible() { m_visible = !m_visible; }

	private:
		void DrawCacheSettings();
		void DrawPreloaderSection();
		void DrawMemoryInfo();
		void DrawMobileOptimization();

		bool m_visible;

		int m_maxModels;
		int m_maxTextures;
		bool m_optimizeForMobile;

		std::string m_preloadPath;
		std::shared_ptr<ModelPreloader> m_preloader;
	};
}

#endif // !SABA_VIEWER_OPTIMIZATIONPANEL_H_