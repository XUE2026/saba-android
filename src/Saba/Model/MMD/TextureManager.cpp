//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "TextureManager.h"
#include <Saba/Base/Log.h>

namespace saba
{
    TextureManager::TextureManager()
    {
    }

    TextureManager::~TextureManager()
    {
        ClearAll();
    }

    void TextureManager::ReplaceTexture(int modelIndex, const std::string& materialName, const std::string& newTexturePath)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto& matReplacements = m_textureReplacements[modelIndex];
        auto it = matReplacements.find(materialName);
        if (it != matReplacements.end())
        {
            it->second.m_replacementPath = newTexturePath;
            SABA_INFO("TextureManager: Replaced texture for model[{}] material[{}] -> [{}]",
                modelIndex, materialName, newTexturePath);
        }
        else
        {
            TextureReplacement replacement;
            replacement.m_replacementPath = newTexturePath;
            matReplacements[materialName] = replacement;
            SABA_INFO("TextureManager: Set texture replacement for model[{}] material[{}] -> [{}]",
                modelIndex, materialName, newTexturePath);
        }
    }

    void TextureManager::ReplaceTexture(int modelIndex, int materialIndex, const std::string& newTexturePath)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto& matReplacements = m_textureReplacements[modelIndex];
        std::string key = std::to_string(materialIndex);
        auto it = matReplacements.find(key);
        if (it != matReplacements.end())
        {
            it->second.m_replacementPath = newTexturePath;
            SABA_INFO("TextureManager: Replaced texture for model[{}] material[{}] -> [{}]",
                modelIndex, materialIndex, newTexturePath);
        }
        else
        {
            TextureReplacement replacement;
            replacement.m_replacementPath = newTexturePath;
            matReplacements[key] = replacement;
            SABA_INFO("TextureManager: Set texture replacement for model[{}] material[{}] -> [{}]",
                modelIndex, materialIndex, newTexturePath);
        }
    }

    void TextureManager::ResetTexture(int modelIndex, const std::string& materialName)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto modelIt = m_textureReplacements.find(modelIndex);
        if (modelIt != m_textureReplacements.end())
        {
            auto& matReplacements = modelIt->second;
            auto it = matReplacements.find(materialName);
            if (it != matReplacements.end())
            {
                matReplacements.erase(it);
                SABA_INFO("TextureManager: Reset texture for model[{}] material[{}]",
                    modelIndex, materialName);
            }
        }
    }

    void TextureManager::ResetTexture(int modelIndex, int materialIndex)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto modelIt = m_textureReplacements.find(modelIndex);
        if (modelIt != m_textureReplacements.end())
        {
            std::string key = std::to_string(materialIndex);
            auto& matReplacements = modelIt->second;
            auto it = matReplacements.find(key);
            if (it != matReplacements.end())
            {
                matReplacements.erase(it);
                SABA_INFO("TextureManager: Reset texture for model[{}] material[{}]",
                    modelIndex, materialIndex);
            }
        }
    }

    void TextureManager::ResetAllTextures(int modelIndex)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto modelIt = m_textureReplacements.find(modelIndex);
        if (modelIt != m_textureReplacements.end())
        {
            modelIt->second.clear();
            SABA_INFO("TextureManager: Reset all textures for model[{}]", modelIndex);
        }
    }

    bool TextureManager::HasReplacement(int modelIndex, const std::string& materialName) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto modelIt = m_textureReplacements.find(modelIndex);
        if (modelIt != m_textureReplacements.end())
        {
            return modelIt->second.find(materialName) != modelIt->second.end();
        }
        return false;
    }

    bool TextureManager::HasReplacement(int modelIndex, int materialIndex) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto modelIt = m_textureReplacements.find(modelIndex);
        if (modelIt != m_textureReplacements.end())
        {
            std::string key = std::to_string(materialIndex);
            return modelIt->second.find(key) != modelIt->second.end();
        }
        return false;
    }

    std::string TextureManager::GetReplacementPath(int modelIndex, const std::string& materialName) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto modelIt = m_textureReplacements.find(modelIndex);
        if (modelIt != m_textureReplacements.end())
        {
            auto it = modelIt->second.find(materialName);
            if (it != modelIt->second.end())
            {
                return it->second.m_replacementPath;
            }
        }
        return {};
    }

    std::string TextureManager::GetReplacementPath(int modelIndex, int materialIndex) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto modelIt = m_textureReplacements.find(modelIndex);
        if (modelIt != m_textureReplacements.end())
        {
            std::string key = std::to_string(materialIndex);
            auto it = modelIt->second.find(key);
            if (it != modelIt->second.end())
            {
                return it->second.m_replacementPath;
            }
        }
        return {};
    }

    std::vector<std::string> TextureManager::GetModifiedMaterialNames(int modelIndex) const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<std::string> names;
        auto modelIt = m_textureReplacements.find(modelIndex);
        if (modelIt != m_textureReplacements.end())
        {
            names.reserve(modelIt->second.size());
            for (const auto& entry : modelIt->second)
            {
                names.push_back(entry.first);
            }
        }
        return names;
    }

    void TextureManager::ClearModel(int modelIndex)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto modelIt = m_textureReplacements.find(modelIndex);
        if (modelIt != m_textureReplacements.end())
        {
            m_textureReplacements.erase(modelIt);
            SABA_INFO("TextureManager: Cleared model[{}]", modelIndex);
        }
    }

    void TextureManager::ClearAll()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_textureReplacements.clear();
        SABA_INFO("TextureManager: Cleared all");
    }
}