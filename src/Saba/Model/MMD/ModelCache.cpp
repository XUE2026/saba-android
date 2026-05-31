//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "ModelCache.h"

#include <Saba/Model/MMD/PMXFile.h>
#include <Saba/Model/MMD/PMXModel.h>

namespace saba
{
	ModelCache::ModelCache(size_t maxSize)
		: m_maxSize(maxSize)
	{
	}

	ModelCache::~ModelCache()
	{
		Clear();
	}

	std::shared_ptr<PMXModel> ModelCache::Get(const std::string& path)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_cacheMap.find(path);
		if (it != m_cacheMap.end())
		{
			m_cacheList.splice(m_cacheList.begin(), m_cacheList, it->second);
			return it->second->model;
		}

		return nullptr;
	}

	void ModelCache::Put(const std::string& path, std::shared_ptr<PMXModel> model)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_cacheMap.find(path);
		if (it != m_cacheMap.end())
		{
			it->second->model = model;
			m_cacheList.splice(m_cacheList.begin(), m_cacheList, it->second);
			return;
		}

		if (m_cacheList.size() >= m_maxSize)
		{
			m_cacheMap.erase(m_cacheList.back().path);
			m_cacheList.pop_back();
		}

		m_cacheList.push_front({ path, model });
		m_cacheMap[path] = m_cacheList.begin();
	}

	void ModelCache::Clear()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_cacheList.clear();
		m_cacheMap.clear();
	}

	size_t ModelCache::GetSize() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_cacheList.size();
	}
}