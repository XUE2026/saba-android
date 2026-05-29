//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "GLMMDClothingManager.h"

#include <Saba/Base/Log.h>
#include <Saba/Base/Path.h>
#include <Saba/GL/GLTextureUtil.h>

#include <glm/glm.hpp>
#include <algorithm>

namespace saba
{
    GLMMDClothingManager::GLMMDClothingManager()
    {
    }

    GLMMDClothingManager::~GLMMDClothingManager()
    {
        ClearAll();
    }

    bool GLMMDClothingManager::LoadClothingMesh(GLMMDClothingItem* item, const std::string& pmxPath)
    {
        PMXFile pmx;
        if (!ReadPMXFile(&pmx, pmxPath.c_str()))
        {
            SABA_ERROR("GLMMDClothingManager: Failed to load PMX file [{}]", pmxPath);
            return false;
        }

        std::string dirPath = PathUtil::GetDirectoryName(pmxPath);

        size_t vertexCount = pmx.m_vertices.size();
        item->m_positions.resize(vertexCount);
        item->m_normals.resize(vertexCount);
        item->m_uvs.resize(vertexCount);

        for (size_t i = 0; i < vertexCount; i++)
        {
            const auto& v = pmx.m_vertices[i];
            item->m_positions[i] = v.m_position * glm::vec3(1, 1, -1);
            item->m_normals[i] = v.m_normal * glm::vec3(1, 1, -1);
            item->m_uvs[i] = glm::vec2(v.m_uv.x, 1.0f - v.m_uv.y);
        }

        item->m_indexTypeSize = pmx.m_header.m_vertexIndexSize;
        item->m_indexCount = pmx.m_faces.size() * 3;

        std::vector<char> indices(item->m_indexCount * item->m_indexTypeSize);
        switch (item->m_indexTypeSize)
        {
        case 1:
            item->m_indexType = GL_UNSIGNED_BYTE;
            {
                int idx = 0;
                uint8_t* dst = (uint8_t*)indices.data();
                for (const auto& face : pmx.m_faces)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        dst[idx] = (uint8_t)face.m_vertices[2 - i];
                        idx++;
                    }
                }
            }
            break;
        case 2:
            item->m_indexType = GL_UNSIGNED_SHORT;
            {
                int idx = 0;
                uint16_t* dst = (uint16_t*)indices.data();
                for (const auto& face : pmx.m_faces)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        dst[idx] = (uint16_t)face.m_vertices[2 - i];
                        idx++;
                    }
                }
            }
            break;
        case 4:
            item->m_indexType = GL_UNSIGNED_INT;
            {
                int idx = 0;
                uint32_t* dst = (uint32_t*)indices.data();
                for (const auto& face : pmx.m_faces)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        dst[idx] = (uint32_t)face.m_vertices[2 - i];
                        idx++;
                    }
                }
            }
            break;
        default:
            SABA_ERROR("GLMMDClothingManager: Unsupported index size [{}]", item->m_indexTypeSize);
            return false;
        }

        std::vector<std::string> texturePaths;
        texturePaths.reserve(pmx.m_textures.size());
        for (const auto& pmxTex : pmx.m_textures)
        {
            std::string texPath = PathUtil::Combine(dirPath, pmxTex.m_textureName);
            texturePaths.emplace_back(std::move(texPath));
        }

        item->m_materials.reserve(pmx.m_materials.size());
        item->m_subMeshes.reserve(pmx.m_materials.size());
        uint32_t beginIndex = 0;

        for (const auto& pmxMat : pmx.m_materials)
        {
            GLMMDClothingMaterial mat;
            mat.m_diffuse = glm::vec3(pmxMat.m_diffuse);
            mat.m_alpha = pmxMat.m_diffuse.a;
            mat.m_specularPower = pmxMat.m_specularPower;
            mat.m_specular = pmxMat.m_specular;
            mat.m_ambient = pmxMat.m_ambient;
            mat.m_bothFace = !!((uint8_t)pmxMat.m_drawMode & (uint8_t)PMXDrawModeFlags::BothFace);
            mat.m_edgeFlag = ((uint8_t)pmxMat.m_drawMode & (uint8_t)PMXDrawModeFlags::DrawEdge) != 0;
            mat.m_edgeSize = pmxMat.m_edgeSize;
            mat.m_edgeColor = pmxMat.m_edgeColor;
            mat.m_spTextureMode = MMDMaterial::SphereTextureMode::None;

            if (pmxMat.m_textureIndex != -1 && (size_t)pmxMat.m_textureIndex < texturePaths.size())
            {
                std::string texPath = PathUtil::Normalize(texturePaths[pmxMat.m_textureIndex]);
                auto texObj = CreateTextureFromFile(texPath.c_str(), true, true);
                if (texObj.Get() != 0)
                {
                    mat.m_texture = std::move(texObj);
                    mat.m_textureHaveAlpha = IsAlphaTexture(mat.m_texture);
                }
            }

            if (pmxMat.m_sphereTextureIndex != -1 && (size_t)pmxMat.m_sphereTextureIndex < texturePaths.size())
            {
                std::string texPath = PathUtil::Normalize(texturePaths[pmxMat.m_sphereTextureIndex]);
                auto texObj = CreateTextureFromFile(texPath.c_str(), false, true);
                if (texObj.Get() != 0)
                {
                    mat.m_spTexture = std::move(texObj);
                    if (pmxMat.m_sphereMode == PMXSphereMode::Mul)
                    {
                        mat.m_spTextureMode = MMDMaterial::SphereTextureMode::Mul;
                    }
                    else if (pmxMat.m_sphereMode == PMXSphereMode::Add)
                    {
                        mat.m_spTextureMode = MMDMaterial::SphereTextureMode::Add;
                    }
                }
            }

            if (pmxMat.m_toonMode == PMXToonMode::Common)
            {
                if (pmxMat.m_toonTextureIndex != -1)
                {
                    char buf[32];
                    std::snprintf(buf, sizeof(buf), "toon%02d.bmp", pmxMat.m_toonTextureIndex + 1);
                    std::string texPath = PathUtil::Combine(
                        PathUtil::GetDirectoryName(pmxPath), buf);
                    auto texObj = CreateTextureFromFile(texPath.c_str());
                    if (texObj.Get() != 0)
                    {
                        mat.m_toonTexture = std::move(texObj);
                    }
                }
            }
            else if (pmxMat.m_toonMode == PMXToonMode::Separate)
            {
                if (pmxMat.m_toonTextureIndex != -1 &&
                    (size_t)pmxMat.m_toonTextureIndex < texturePaths.size())
                {
                    std::string texPath = PathUtil::Normalize(texturePaths[pmxMat.m_toonTextureIndex]);
                    auto texObj = CreateTextureFromFile(texPath.c_str());
                    if (texObj.Get() != 0)
                    {
                        mat.m_toonTexture = std::move(texObj);
                    }
                }
            }

            item->m_materials.emplace_back(std::move(mat));

            MMDSubMesh subMesh;
            subMesh.m_beginIndex = beginIndex;
            subMesh.m_vertexCount = pmxMat.m_numFaceVertices;
            subMesh.m_materialID = (int)(item->m_materials.size() - 1);
            item->m_subMeshes.push_back(subMesh);

            beginIndex += pmxMat.m_numFaceVertices;
        }

        item->m_posVBO = CreateVBO(item->m_positions.data(), vertexCount, GL_STATIC_DRAW);
        item->m_norVBO = CreateVBO(item->m_normals.data(), vertexCount, GL_STATIC_DRAW);
        item->m_uvVBO = CreateVBO(item->m_uvs.data(), vertexCount, GL_STATIC_DRAW);

        item->m_posBinder = MakeVertexBinder<glm::vec3>();
        item->m_norBinder = MakeVertexBinder<glm::vec3>();
        item->m_uvBinder = MakeVertexBinder<glm::vec2>();

        switch (item->m_indexTypeSize)
        {
        case 1:
            item->m_ibo = CreateIBO((const uint8_t*)indices.data(), item->m_indexCount, GL_STATIC_DRAW);
            break;
        case 2:
            item->m_ibo = CreateIBO((const uint16_t*)indices.data(), item->m_indexCount, GL_STATIC_DRAW);
            break;
        case 4:
            item->m_ibo = CreateIBO((const uint32_t*)indices.data(), item->m_indexCount, GL_STATIC_DRAW);
            break;
        }

        item->m_vaos.resize(item->m_subMeshes.size());
        for (size_t i = 0; i < item->m_vaos.size(); i++)
        {
            if (!item->m_vaos[i].Create())
            {
                SABA_ERROR("GLMMDClothingManager: VAO creation failed for clothing [{}]", item->m_name);
                return false;
            }

            glBindVertexArray(item->m_vaos[i]);
            item->m_posBinder.Bind(0, item->m_posVBO);
            glEnableVertexAttribArray(0);
            item->m_norBinder.Bind(1, item->m_norVBO);
            glEnableVertexAttribArray(1);
            item->m_uvBinder.Bind(2, item->m_uvVBO);
            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, item->m_ibo);
            glBindVertexArray(0);
        }

        SABA_INFO("GLMMDClothingManager: Loaded clothing mesh [{}] with {} vertices, {} faces, {} materials",
            item->m_name, vertexCount, pmx.m_faces.size(), item->m_materials.size());

        return true;
    }

    int GLMMDClothingManager::AddClothing(int modelIndex, const std::string& clothingPmxPath, const std::string& boneAttachment)
    {
        auto item = std::make_unique<GLMMDClothingItem>();
        item->m_name = PathUtil::GetFilenameWithoutExt(clothingPmxPath);
        item->m_sourcePath = clothingPmxPath;
        item->m_boneAttachment = boneAttachment;
        item->m_visible = true;

        if (!LoadClothingMesh(item.get(), clothingPmxPath))
        {
            SABA_ERROR("GLMMDClothingManager: Failed to load clothing PMX [{}]", clothingPmxPath);
            return -1;
        }

        auto& modelData = m_clothingData[modelIndex];
        int id = modelData.m_nextId++;
        item->m_id = id;
        modelData.m_items[id] = std::move(item);

        SABA_INFO("GLMMDClothingManager: Added clothing [{}] (id={}) to model[{}] attached to bone [{}]",
            clothingPmxPath, id, modelIndex, boneAttachment);

        return id;
    }

    bool GLMMDClothingManager::RemoveClothing(int modelIndex, int clothingId)
    {
        auto modelIt = m_clothingData.find(modelIndex);
        if (modelIt == m_clothingData.end())
        {
            SABA_WARN("GLMMDClothingManager: Model [{}] not found", modelIndex);
            return false;
        }

        auto& items = modelIt->second.m_items;
        auto itemIt = items.find(clothingId);
        if (itemIt == items.end())
        {
            SABA_WARN("GLMMDClothingManager: Clothing id [{}] not found in model [{}]", clothingId, modelIndex);
            return false;
        }

        items.erase(itemIt);
        SABA_INFO("GLMMDClothingManager: Removed clothing id [{}] from model [{}]", clothingId, modelIndex);
        return true;
    }

    bool GLMMDClothingManager::ToggleClothingVisibility(int modelIndex, int clothingId)
    {
        auto item = GetClothing(modelIndex, clothingId);
        if (item == nullptr)
        {
            return false;
        }
        item->m_visible = !item->m_visible;
        SABA_INFO("GLMMDClothingManager: Toggled visibility of clothing id [{}] to [{}]",
            clothingId, item->m_visible);
        return true;
    }

    bool GLMMDClothingManager::SetClothingVisibility(int modelIndex, int clothingId, bool visible)
    {
        auto item = GetClothing(modelIndex, clothingId);
        if (item == nullptr)
        {
            return false;
        }
        item->m_visible = visible;
        return true;
    }

    const GLMMDClothingItem* GLMMDClothingManager::GetClothing(int modelIndex, int clothingId) const
    {
        auto modelIt = m_clothingData.find(modelIndex);
        if (modelIt == m_clothingData.end())
        {
            return nullptr;
        }

        auto itemIt = modelIt->second.m_items.find(clothingId);
        if (itemIt == modelIt->second.m_items.end())
        {
            return nullptr;
        }

        return itemIt->second.get();
    }

    GLMMDClothingItem* GLMMDClothingManager::GetClothing(int modelIndex, int clothingId)
    {
        auto modelIt = m_clothingData.find(modelIndex);
        if (modelIt == m_clothingData.end())
        {
            return nullptr;
        }

        auto itemIt = modelIt->second.m_items.find(clothingId);
        if (itemIt == modelIt->second.m_items.end())
        {
            return nullptr;
        }

        return itemIt->second.get();
    }

    std::vector<int> GLMMDClothingManager::GetClothingIds(int modelIndex) const
    {
        std::vector<int> ids;
        auto modelIt = m_clothingData.find(modelIndex);
        if (modelIt != m_clothingData.end())
        {
            ids.reserve(modelIt->second.m_items.size());
            for (const auto& entry : modelIt->second.m_items)
            {
                ids.push_back(entry.first);
            }
        }
        return ids;
    }

    size_t GLMMDClothingManager::GetClothingCount(int modelIndex) const
    {
        auto modelIt = m_clothingData.find(modelIndex);
        if (modelIt != m_clothingData.end())
        {
            return modelIt->second.m_items.size();
        }
        return 0;
    }

    void GLMMDClothingManager::ClearModel(int modelIndex)
    {
        auto modelIt = m_clothingData.find(modelIndex);
        if (modelIt != m_clothingData.end())
        {
            m_clothingData.erase(modelIt);
            SABA_INFO("GLMMDClothingManager: Cleared all clothing for model [{}]", modelIndex);
        }
    }

    void GLMMDClothingManager::ClearAll()
    {
        m_clothingData.clear();
        SABA_INFO("GLMMDClothingManager: Cleared all clothing data");
    }
}