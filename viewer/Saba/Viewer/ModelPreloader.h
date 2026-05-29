//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_MODELPRELOADER_H_
#define SABA_VIEWER_MODELPRELOADER_H_

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <future>
#include <functional>

namespace saba
{
	class ModelPreloader
	{
	public:
		ModelPreloader();
		~ModelPreloader();

		ModelPreloader(const ModelPreloader&) = delete;
		ModelPreloader& operator=(const ModelPreloader&) = delete;

		void PreloadModel(const std::string& path);
		void PreloadTextures(const std::string& textureDir, const std::vector<std::string>& texturePaths);

		float GetPreloadProgress() const;
		bool IsPreloadComplete() const;
		void CancelPreload();
		void WaitForCompletion();

		using ProgressCallback = std::function<void(float progress, const std::string& currentTask)>;
		void SetProgressCallback(ProgressCallback callback);

	private:
		void RunPreloadTask(const std::string& path);

		std::atomic<float>				m_progress;
		std::atomic<bool>				m_cancelled;
		std::future<void>				m_future;
		ProgressCallback				m_progressCallback;
	};
}

#endif // !SABA_VIEWER_MODELPRELOADER_H_