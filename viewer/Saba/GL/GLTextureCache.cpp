//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "GLTextureCache.h"
#include "GLTextureUtil.h"

#include <GL/gl3w.h>

namespace saba
{
	GLTextureCache::GLTextureCache(size_t maxSize)
		: m_maxSize(maxSize)
	{
	}

	GLTextureCache::~GLTextureCache()
	{
		Clear();
	}

	GLuint GLTextureCache::Get(const std::string& path)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_cacheMap.find(path);
		if (it != m_cacheMap.end())
		{
			m_cacheList.splice(m_cacheList.begin(), m_cacheList, it->second);
			return it->second->texture;
		}

		GLTextureObject texObj = CreateTextureFromFile(path.c_str(), true, false);
		if (texObj.Get() == 0)
		{
			return 0;
		}

		if (m_cacheList.size() >= m_maxSize)
		{
			CacheEntry& last = m_cacheList.back();
			GLuint lastTex = last.texture;
			glDeleteTextures(1, &lastTex);
			m_cacheMap.erase(last.path);
			m_cacheList.pop_back();
		}

		m_cacheList.push_front({ path, texObj.Release() });
		m_cacheMap[path] = m_cacheList.begin();

		return m_cacheList.front().texture;
	}

	void GLTextureCache::Put(const std::string& path, GLuint tex)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_cacheMap.find(path);
		if (it != m_cacheMap.end())
		{
			GLuint oldTex = it->second->texture;
			if (oldTex != tex)
			{
				glDeleteTextures(1, &oldTex);
			}
			it->second->texture = tex;
			m_cacheList.splice(m_cacheList.begin(), m_cacheList, it->second);
			return;
		}

		if (m_cacheList.size() >= m_maxSize)
		{
			CacheEntry& last = m_cacheList.back();
			GLuint lastTex = last.texture;
			glDeleteTextures(1, &lastTex);
			m_cacheMap.erase(last.path);
			m_cacheList.pop_back();
		}

		m_cacheList.push_front({ path, tex });
		m_cacheMap[path] = m_cacheList.begin();
	}

	void GLTextureCache::Clear()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		for (auto& entry : m_cacheList)
		{
			if (entry.texture != 0)
			{
				glDeleteTextures(1, &entry.texture);
			}
		}
		m_cacheList.clear();
		m_cacheMap.clear();
	}

	size_t GLTextureCache::GetSize() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_cacheList.size();
	}
}