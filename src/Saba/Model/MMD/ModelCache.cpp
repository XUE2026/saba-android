//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "ModelCache.h"
#include "PMXModel.h"
#include "PMDModel.h"
#include <Saba/Base/Log.h>
#include <Saba/Base/Path.h>

#include <algorithm>

namespace saba
{
	ModelCache::ModelCache()
		: m_maxModels(5)
	{
	}

	ModelCache::~ModelCache()
	{
		Clear();
	}

	ModelCache& ModelCache::Get()
	{
		static ModelCache s_instance;
		return s_instance;
	}

	void ModelCache::SetMaxModels(size_t maxCount)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_maxModels = maxCount;
		while (m_cache.size() > m_maxModels)
		{
			EvictLRU();
		}
	}

	size_t ModelCache::GetMaxModels() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_maxModels;
	}

	size_t ModelCache::GetModelCount() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_cache.size();
	}

	void ModelCache::SetProgressCallback(ProgressCallback callback)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_progressCallback = std::move(callback);
	}

	std::shared_ptr<MMDModel> ModelCache::LoadOrGet(const std::string& path)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_pathIndexMap.find(path);
		if (it != m_pathIndexMap.end())
		{
			size_t index = it->second;
			if (index < m_cache.size())
			{
				m_cache[index].m_lastAccess = std::chrono::steady_clock::now();
				SABA_INFO("ModelCache: Cache hit for [{}]", path);
				return m_cache[index].m_model;
			}
			m_pathIndexMap.erase(it);
		}

		SABA_INFO("ModelCache: Cache miss for [{}], loading...", path);

		if (m_progressCallback)
		{
			m_progressCallback(path, 0.0f);
		}

		std::string ext = PathUtil::GetExt(path);
		std::shared_ptr<MMDModel> model;

		if (ext == "pmx" || ext == "PMX")
		{
			auto pmxModel = std::make_shared<PMXModel>();
			std::string dirPath = PathUtil::GetDirectoryName(path);
			if (!pmxModel->Load(path, dirPath))
			{
				SABA_ERROR("ModelCache: Failed to load PMX model [{}]", path);
				return nullptr;
			}
			model = pmxModel;
		}
		else if (ext == "pmd" || ext == "PMD")
		{
			auto pmdModel = std::make_shared<PMDModel>();
			std::string dirPath = PathUtil::GetDirectoryName(path);
			if (!pmdModel->Load(path, dirPath))
			{
				SABA_ERROR("ModelCache: Failed to load PMD model [{}]", path);
				return nullptr;
			}
			model = pmdModel;
		}
		else
		{
			SABA_ERROR("ModelCache: Unsupported model format [{}]", path);
			return nullptr;
		}

		if (m_progressCallback)
		{
			m_progressCallback(path, 1.0f);
		}

		while (m_cache.size() >= m_maxModels)
		{
			EvictLRU();
		}

		CacheEntry entry;
		entry.m_path = path;
		entry.m_model = model;
		entry.m_lastAccess = std::chrono::steady_clock::now();

		size_t newIndex = m_cache.size();
		m_cache.push_back(std::move(entry));
		m_pathIndexMap[path] = newIndex;

		SABA_INFO("ModelCache: Cached model [{}] (total: {})", path, m_cache.size());
		return model;
	}

	void ModelCache::Evict(const std::string& path)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_pathIndexMap.find(path);
		if (it == m_pathIndexMap.end())
		{
			return;
		}

		size_t index = it->second;
		m_pathIndexMap.erase(it);

		m_cache.erase(m_cache.begin() + index);

		for (auto& pair : m_pathIndexMap)
		{
			if (pair.second > index)
			{
				pair.second--;
			}
		}

		SABA_INFO("ModelCache: Evicted model [{}]", path);
	}

	void ModelCache::Clear()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_cache.clear();
		m_pathIndexMap.clear();
		SABA_INFO("ModelCache: Cleared all cached models");
	}

	void ModelCache::EvictLRU()
	{
		if (m_cache.empty())
		{
			return;
		}

		auto lruIt = std::min_element(
			m_cache.begin(),
			m_cache.end(),
			[](const CacheEntry& a, const CacheEntry& b)
			{
				return a.m_lastAccess < b.m_lastAccess;
			}
		);

		size_t lruIndex = lruIt - m_cache.begin();
		std::string lruPath = lruIt->m_path;

		m_pathIndexMap.erase(lruPath);
		m_cache.erase(lruIt);

		for (auto& pair : m_pathIndexMap)
		{
			if (pair.second > lruIndex)
			{
				pair.second--;
			}
		}

		SABA_INFO("ModelCache: LRU eviction of [{}]", lruPath);
	}
}