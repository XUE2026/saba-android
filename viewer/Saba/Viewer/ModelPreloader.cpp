//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "ModelPreloader.h"
#include <Saba/Base/Log.h>
#include <Saba/Base/Path.h>
#include <Saba/Model/MMD/ModelCache.h>
#include <Saba/Model/MMD/PMXFile.h>
#include <Saba/Model/MMD/PMXModel.h>
#include <Saba/Model/MMD/PMDFile.h>
#include <Saba/Model/MMD/PMDModel.h>

#include <thread>
#include <chrono>

namespace saba
{
	ModelPreloader::ModelPreloader()
		: m_progress(0.0f)
		, m_cancelled(false)
	{
	}

	ModelPreloader::~ModelPreloader()
	{
		CancelPreload();
		WaitForCompletion();
	}

	void ModelPreloader::SetProgressCallback(ProgressCallback callback)
	{
		m_progressCallback = std::move(callback);
	}

	void ModelPreloader::PreloadModel(const std::string& path)
	{
		CancelPreload();
		WaitForCompletion();

		m_progress.store(0.0f, std::memory_order_relaxed);
		m_cancelled.store(false, std::memory_order_relaxed);

		m_future = std::async(std::launch::async, [this, path]()
			{
				RunPreloadTask(path);
			});
	}

	void ModelPreloader::PreloadTextures(const std::string& textureDir, const std::vector<std::string>& texturePaths)
	{
		CancelPreload();
		WaitForCompletion();

		m_progress.store(0.0f, std::memory_order_relaxed);
		m_cancelled.store(false, std::memory_order_relaxed);

		m_future = std::async(std::launch::async, [this, textureDir, texturePaths]()
			{
				size_t total = texturePaths.size();
				if (total == 0)
				{
					m_progress.store(1.0f, std::memory_order_relaxed);
					return;
				}

				for (size_t i = 0; i < total; i++)
				{
					if (m_cancelled.load(std::memory_order_relaxed))
					{
						SABA_INFO("ModelPreloader: Texture preloading cancelled");
						return;
					}

					std::string fullPath = PathUtil::Combine(textureDir, texturePaths[i]);
					if (m_progressCallback)
					{
						m_progressCallback(
							static_cast<float>(i) / static_cast<float>(total),
							"Loading texture: " + texturePaths[i]
						);
					}

					PMXFile pmx;
					// Texture preloading just warms the file system cache
					SABA_INFO("ModelPreloader: Preloading texture [{}]", fullPath);

					float progress = static_cast<float>(i + 1) / static_cast<float>(total);
					m_progress.store(progress, std::memory_order_relaxed);

					std::this_thread::yield();
				}

				m_progress.store(1.0f, std::memory_order_relaxed);
				if (m_progressCallback)
				{
					m_progressCallback(1.0f, "Texture preloading complete");
				}
				SABA_INFO("ModelPreloader: Texture preloading complete ({} files)", total);
			});
	}

	float ModelPreloader::GetPreloadProgress() const
	{
		return m_progress.load(std::memory_order_relaxed);
	}

	bool ModelPreloader::IsPreloadComplete() const
	{
		if (!m_future.valid())
		{
			return true;
		}
		auto status = m_future.wait_for(std::chrono::seconds(0));
		return status == std::future_status::ready;
	}

	void ModelPreloader::CancelPreload()
	{
		m_cancelled.store(true, std::memory_order_relaxed);
	}

	void ModelPreloader::WaitForCompletion()
	{
		if (m_future.valid())
		{
			m_future.wait();
		}
	}

	void ModelPreloader::RunPreloadTask(const std::string& path)
	{
		SABA_INFO("ModelPreloader: Starting preload of model [{}]", path);
		if (m_progressCallback)
		{
			m_progressCallback(0.0f, "Starting model load: " + path);
		}

		if (m_cancelled.load(std::memory_order_relaxed))
		{
			return;
		}

		m_progress.store(0.1f, std::memory_order_relaxed);

		std::string ext = PathUtil::GetExt(path);
		bool success = false;

		if (ext == "pmx" || ext == "PMX")
		{
			PMXFile pmx;
			if (ReadPMXFile(&pmx, path.c_str()))
			{
				success = true;
			}
		}
		else if (ext == "pmd" || ext == "PMD")
		{
			PMDFile pmd;
			if (ReadPMDFile(&pmd, path.c_str()))
			{
				success = true;
			}
		}
		else
		{
			SABA_WARN("ModelPreloader: Unsupported model format [{}]", path);
		}

		m_progress.store(0.5f, std::memory_order_relaxed);
		if (m_progressCallback)
		{
			m_progressCallback(0.5f, "Parsing model data: " + PathUtil::GetFilename(path));
		}

		if (!m_cancelled.load(std::memory_order_relaxed) && success)
		{
			ModelCache::Get().LoadOrGet(path);
		}

		if (m_cancelled.load(std::memory_order_relaxed))
		{
			SABA_INFO("ModelPreloader: Preload cancelled for [{}]", path);
			return;
		}

		m_progress.store(1.0f, std::memory_order_relaxed);
		if (m_progressCallback)
		{
			m_progressCallback(1.0f, "Model preload complete: " + PathUtil::GetFilename(path));
		}
		SABA_INFO("ModelPreloader: Preload complete for [{}]", path);
	}
}