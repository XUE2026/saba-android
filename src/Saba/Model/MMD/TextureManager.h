//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_MODEL_MMD_TEXTUREMANAGER_H_
#define SABA_MODEL_MMD_TEXTUREMANAGER_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace saba
{
	class TextureManager
	{
	public:
		TextureManager();
		~TextureManager();

		bool ReplaceTexture(const std::string& modelPath, const std::string& oldTex, const std::string& newTex);
		bool ResetTexture(const std::string& modelPath);
		std::vector<std::string> GetTextureList(const std::string& modelPath) const;

	private:
		struct ModelTextureInfo
		{
			std::vector<std::string> originalTextures;
			std::vector<std::string> replacedTextures;
		};

		using TextureMap = std::unordered_map<std::string, ModelTextureInfo>;

		TextureMap m_textureMap;
		mutable std::mutex m_mutex;
	};
}

#endif // !SABA_MODEL_MMD_TEXTUREMANAGER_H_