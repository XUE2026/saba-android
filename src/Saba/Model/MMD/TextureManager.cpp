//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "TextureManager.h"

#include <algorithm>

namespace saba
{
	TextureManager::TextureManager()
	{
	}

	TextureManager::~TextureManager()
	{
	}

	bool TextureManager::ReplaceTexture(const std::string& modelPath, const std::string& oldTex, const std::string& newTex)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto& info = m_textureMap[modelPath];
		if (info.originalTextures.empty())
		{
			info.originalTextures.push_back(oldTex);
		}

		auto it = std::find(info.replacedTextures.begin(), info.replacedTextures.end(), oldTex);
		if (it != info.replacedTextures.end())
		{
			*it = newTex;
		}
		else
		{
			info.replacedTextures.push_back(newTex);
		}

		return true;
	}

	bool TextureManager::ResetTexture(const std::string& modelPath)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_textureMap.find(modelPath);
		if (it == m_textureMap.end())
		{
			return false;
		}

		it->second.replacedTextures.clear();
		return true;
	}

	std::vector<std::string> TextureManager::GetTextureList(const std::string& modelPath) const
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		auto it = m_textureMap.find(modelPath);
		if (it == m_textureMap.end())
		{
			return {};
		}

		std::vector<std::string> textures = it->second.originalTextures;
		for (const auto& replaced : it->second.replacedTextures)
		{
			textures.push_back(replaced);
		}
		return textures;
	}
}