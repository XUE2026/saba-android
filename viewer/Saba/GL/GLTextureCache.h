//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_GL_GLTEXTURECACHE_H_
#define SABA_GL_GLTEXTURECACHE_H_

#include "GLObject.h"

#include <string>
#include <list>
#include <unordered_map>
#include <mutex>

namespace saba
{
	class GLTextureCache
	{
	public:
		GLTextureCache(size_t maxSize = 50);
		~GLTextureCache();

		GLuint Get(const std::string& path);
		void Put(const std::string& path, GLuint tex);
		void Clear();
		size_t GetSize() const;

	private:
		struct CacheEntry
		{
			std::string path;
			GLuint texture;
		};

		using CacheList = std::list<CacheEntry>;
		using CacheMap = std::unordered_map<std::string, CacheList::iterator>;

		size_t m_maxSize;
		CacheList m_cacheList;
		CacheMap m_cacheMap;
		mutable std::mutex m_mutex;
	};
}

#endif // !SABA_GL_GLTEXTURECACHE_H_