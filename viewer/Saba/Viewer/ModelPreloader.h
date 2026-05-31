//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_MODELPRELOADER_H_
#define SABA_VIEWER_MODELPRELOADER_H_

#include <string>
#include <future>
#include <memory>
#include <functional>
#include <atomic>

namespace saba
{
	class ModelPreloader
	{
	public:
		ModelPreloader();
		~ModelPreloader();

		void PreloadModel(const std::string& path);
		float GetProgress() const;
		bool IsComplete() const;
		bool WasSuccessful() const;
		void Cancel();
		void SetCallback(std::function<void(bool)> callback);

	private:
		void LoadTask(const std::string& path);

		std::future<void> m_future;
		std::atomic<float> m_progress;
		std::atomic<bool> m_complete;
		std::atomic<bool> m_success;
		std::atomic<bool> m_cancelled;
		std::function<void(bool)> m_callback;
	};
}

#endif // !SABA_VIEWER_MODELPRELOADER_H_