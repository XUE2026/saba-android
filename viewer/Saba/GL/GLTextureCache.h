//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_GL_TEXTURECACHE_H_
#define SABA_GL_TEXTURECACHE_H_

#include <Saba/GL/GLObject.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <chrono>

namespace saba
{
	class GLTextureCache
	{
	public:
		static GLTextureCache& Get();

		GLTextureCache(const GLTextureCache&) = delete;
		GLTextureCache& operator=(const GLTextureCache&) = delete;

		GLuint LoadOrGetTexture(const std::string& path, bool genMipMap = true, bool rgba = false);
		void Invalidate(const std::string& path);
		void Clear();

		void SetMaxTextures(size_t maxCount);
		size_t GetMaxTextures() const;
		size_t GetTextureCount() const;

		size_t GetEstimatedMemoryUsage() const;

	private:
		GLTextureCache();
		~GLTextureCache();

		struct CacheEntry
		{
			std::string								m_path;
			GLTextureObject							m_texture;
			bool									m_genMipMap;
			bool									m_rgba;
			size_t									m_estimatedSize;
			std::chrono::steady_clock::time_point	m_lastAccess;
		};

		void EvictLRU();

		mutable std::mutex						m_mutex;
		size_t									m_maxTextures;
		std::vector<CacheEntry>					m_cache;
		std::unordered_map<std::string, size_t>	m_pathIndexMap;
	};
}

#endif // !SABA_GL_TEXTURECACHE_H_