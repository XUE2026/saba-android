//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_MODEL_MMD_MODELCACHE_H_
#define SABA_MODEL_MMD_MODELCACHE_H_

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <functional>

namespace saba
{
	class MMDModel;

	class ModelCache
	{
	public:
		static ModelCache& Get();

		ModelCache(const ModelCache&) = delete;
		ModelCache& operator=(const ModelCache&) = delete;

		std::shared_ptr<MMDModel> LoadOrGet(const std::string& path);
		void Evict(const std::string& path);
		void Clear();

		void SetMaxModels(size_t maxCount);
		size_t GetMaxModels() const;
		size_t GetModelCount() const;

		using ProgressCallback = std::function<void(const std::string& path, float progress)>;
		void SetProgressCallback(ProgressCallback callback);

	private:
		ModelCache();
		~ModelCache();

		struct CacheEntry
		{
			std::string								m_path;
			std::shared_ptr<MMDModel>				m_model;
			std::chrono::steady_clock::time_point	m_lastAccess;
		};

		void EvictLRU();

		mutable std::mutex						m_mutex;
		size_t									m_maxModels;
		std::vector<CacheEntry>					m_cache;
		std::unordered_map<std::string, size_t>	m_pathIndexMap;
		ProgressCallback						m_progressCallback;
	};
}

#endif // !SABA_MODEL_MMD_MODELCACHE_H_