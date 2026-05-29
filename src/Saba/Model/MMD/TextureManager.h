//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_MODEL_MMD_TEXTUREMANAGER_H_
#define SABA_MODEL_MMD_TEXTUREMANAGER_H_

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <memory>

namespace saba
{
    class TextureManager
    {
    public:
        TextureManager();
        ~TextureManager();

        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;

        void ReplaceTexture(int modelIndex, const std::string& materialName, const std::string& newTexturePath);
        void ReplaceTexture(int modelIndex, int materialIndex, const std::string& newTexturePath);

        void ResetTexture(int modelIndex, const std::string& materialName);
        void ResetTexture(int modelIndex, int materialIndex);
        void ResetAllTextures(int modelIndex);

        bool HasReplacement(int modelIndex, const std::string& materialName) const;
        bool HasReplacement(int modelIndex, int materialIndex) const;

        std::string GetReplacementPath(int modelIndex, const std::string& materialName) const;
        std::string GetReplacementPath(int modelIndex, int materialIndex) const;

        std::vector<std::string> GetModifiedMaterialNames(int modelIndex) const;

        void ClearModel(int modelIndex);
        void ClearAll();

    private:
        struct TextureReplacement
        {
            std::string m_originalPath;
            std::string m_replacementPath;
        };

        using MaterialReplacementMap = std::map<std::string, TextureReplacement>;
        using ModelTextureMap = std::map<int, MaterialReplacementMap>;

        mutable std::mutex m_mutex;
        ModelTextureMap m_textureReplacements;
    };
}

#endif