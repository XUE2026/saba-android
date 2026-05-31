//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_MODEL_MMD_MODELCACHE_H_
#define SABA_MODEL_MMD_MODELCACHE_H_

#include <string>
#include <list>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace saba
{
	class PMXModel;

	class ModelCache
	{
	public:
		ModelCache(size_t maxSize = 5);
		~ModelCache();

		std::shared_ptr<PMXModel> Get(const std::string& path);
		void Put(const std::string& path, std::shared_ptr<PMXModel> model);
		void Clear();
		size_t GetSize() const;

	private:
		struct CacheEntry
		{
			std::string path;
			std::shared_ptr<PMXModel> model;
		};

		using CacheList = std::list<CacheEntry>;
		using CacheMap = std::unordered_map<std::string, CacheList::iterator>;

		size_t m_maxSize;
		CacheList m_cacheList;
		CacheMap m_cacheMap;
		mutable std::mutex m_mutex;
	};
}

#endif // !SABA_MODEL_MMD_MODELCACHE_H_