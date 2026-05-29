//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "GLTextureCache.h"
#include "GLTextureUtil.h"
#include <Saba/Base/Log.h>

#include <algorithm>

namespace saba
{
	GLTextureCache::GLTextureCache()
		: m_maxTextures(50)
	{
	}

	GLTextureCache::~GLTextureCache()
	{
		Clear();
	}

	GLTextureCache& GLTextureCache::Get()
	{
		static GLTextureCache s_instance;
		return s_instance;
	}

	void GLTextureCache::SetMaxTextures(size_t maxCount)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_maxTextures = maxCount;
		while (m_cache.size() > m_maxTextures)
		{
			EvictLRU();
		}
	}

	size_t GLTextureCache::GetMaxTextures() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_maxTextures;
	}

	size_t GLTextureCache::GetTextureCount() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_cache.size();
	}

	size_t GLTextureCache::GetEstimatedMemoryUsage() const
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		size_t total = 0;
		for (const auto& entry : m_cache)
		{
			total += entry.m_estimatedSize;
		}
		return total;
	}

	GLuint GLTextureCache::LoadOrGetTexture(const std::string& path, bool genMipMap, bool rgba)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		std::string key = path + ":" + std::to_string(genMipMap) + ":" + std::to_string(rgba);
		auto it = m_pathIndexMap.find(key);
		if (it != m_pathIndexMap.end())
		{
			size_t index = it->second;
			if (index < m_cache.size())
			{
				m_cache[index].m_lastAccess = std::chrono::steady_clock::now();
				SABA_INFO("GLTextureCache: Cache hit for [{}]", path);
				return m_cache[index].m_texture.Get();
			}
			m_pathIndexMap.erase(it);
		}

		SABA_INFO("GLTextureCache: Cache miss for [{}], loading...", path);

		GLTextureObject tex;
		if (!tex.Create())
		{
			SABA_ERROR("GLTextureCache: Failed to create texture object for [{}]", path);
			return 0;
		}

		if (!LoadTextureFromFile(tex, path, genMipMap, rgba))
		{
			SABA_ERROR("GLTextureCache: Failed to load texture [{}]", path);
			tex.Destroy();
			return 0;
		}

		size_t estimatedSize = 0;
		glBindTexture(GL_TEXTURE_2D, tex.Get());
		int width = 0, height = 0;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
		int internalFormat = 0;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
		glBindTexture(GL_TEXTURE_2D, 0);

		int bytesPerPixel = 4;
		switch (internalFormat)
		{
		case GL_RGB:
		case GL_RGB8:
			bytesPerPixel = 3;
			break;
		case GL_RGBA:
		case GL_RGBA8:
			bytesPerPixel = 4;
			break;
		case GL_RGB32F:
			bytesPerPixel = 12;
			break;
		case GL_RGBA32F:
			bytesPerPixel = 16;
			break;
		default:
			bytesPerPixel = 4;
			break;
		}
		estimatedSize = static_cast<size_t>(width * height * bytesPerPixel);

		if (genMipMap)
		{
			estimatedSize = estimatedSize * 4 / 3;
		}

		while (m_cache.size() >= m_maxTextures)
		{
			EvictLRU();
		}

		CacheEntry entry;
		entry.m_path = path;
		entry.m_texture = std::move(tex);
		entry.m_genMipMap = genMipMap;
		entry.m_rgba = rgba;
		entry.m_estimatedSize = estimatedSize;
		entry.m_lastAccess = std::chrono::steady_clock::now();

		size_t newIndex = m_cache.size();
		m_cache.push_back(std::move(entry));
		m_pathIndexMap[key] = newIndex;

		SABA_INFO("GLTextureCache: Cached texture [{}] (~{} MB, total: {})",
			path, estimatedSize / (1024 * 1024), m_cache.size());

		return m_cache[newIndex].m_texture.Get();
	}

	void GLTextureCache::Invalidate(const std::string& path)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_pathIndexMap.find(path);
		if (it == m_pathIndexMap.end())
		{
			// Try with default flags
			std::string key = path + ":" + "1:0";
			it = m_pathIndexMap.find(key);
			if (it == m_pathIndexMap.end())
			{
				return;
			}
		}

		size_t index = it->second;
		m_pathIndexMap.erase(it);

		m_cache[index].m_texture.Destroy();
		m_cache.erase(m_cache.begin() + index);

		for (auto& pair : m_pathIndexMap)
		{
			if (pair.second > index)
			{
				pair.second--;
			}
		}

		SABA_INFO("GLTextureCache: Invalidated texture [{}]", path);
	}

	void GLTextureCache::Clear()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		for (auto& entry : m_cache)
		{
			entry.m_texture.Destroy();
		}
		m_cache.clear();
		m_pathIndexMap.clear();
		SABA_INFO("GLTextureCache: Cleared all cached textures");
	}

	void GLTextureCache::EvictLRU()
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
		std::string key = lruIt->m_path + ":" +
			std::to_string(lruIt->m_genMipMap) + ":" +
			std::to_string(lruIt->m_rgba);

		auto mapIt = m_pathIndexMap.find(key);
		if (mapIt != m_pathIndexMap.end())
		{
			m_pathIndexMap.erase(mapIt);
		}

		lruIt->m_texture.Destroy();
		m_cache.erase(lruIt);

		for (auto& pair : m_pathIndexMap)
		{
			if (pair.second > lruIndex)
			{
				pair.second--;
			}
		}

		SABA_INFO("GLTextureCache: LRU eviction of texture [{}]", lruIt->m_path);
	}
}