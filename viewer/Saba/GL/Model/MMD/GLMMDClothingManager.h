//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_GL_MODEL_MMD_GLMMDCLOTHINGMANAGER_H_
#define SABA_GL_MODEL_MMD_GLMMDCLOTHINGMANAGER_H_

#include <Saba/GL/GLObject.h>
#include <Saba/GL/GLVertexUtil.h>
#include <Saba/Model/MMD/PMXFile.h>
#include <Saba/Model/MMD/MMDMaterial.h>
#include <Saba/Model/MMD/MMDNode.h>
#include <Saba/GL/Model/MMD/GLMMDModel.h>

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace saba
{
    struct GLMMDClothingMaterial
    {
        glm::vec3       m_diffuse;
        float           m_alpha;
        float           m_specularPower;
        glm::vec3       m_specular;
        glm::vec3       m_ambient;
        bool            m_bothFace;
        bool            m_edgeFlag;
        float           m_edgeSize;
        glm::vec4       m_edgeColor;
        GLTextureRef    m_texture;
        bool            m_textureHaveAlpha;
        GLTextureRef    m_spTexture;
        MMDMaterial::SphereTextureMode m_spTextureMode;
        GLTextureRef    m_toonTexture;
    };

    struct GLMMDClothingItem
    {
        int             m_id;
        std::string     m_name;
        std::string     m_sourcePath;
        std::string     m_boneAttachment;
        bool            m_visible;

        std::vector<glm::vec3>      m_positions;
        std::vector<glm::vec3>      m_normals;
        std::vector<glm::vec2>      m_uvs;

        GLBufferObject  m_posVBO;
        GLBufferObject  m_norVBO;
        GLBufferObject  m_uvVBO;

        VertexBinder    m_posBinder;
        VertexBinder    m_norBinder;
        VertexBinder    m_uvBinder;

        GLenum          m_indexType;
        size_t          m_indexTypeSize;
        GLBufferObject  m_ibo;
        size_t          m_indexCount;

        std::vector<GLMMDClothingMaterial>  m_materials;
        std::vector<MMDSubMesh>             m_subMeshes;
        std::vector<GLVertexArrayObject>    m_vaos;

        GLMMDClothingItem()
            : m_id(0)
            , m_visible(true)
            , m_indexType(GL_UNSIGNED_INT)
            , m_indexTypeSize(4)
            , m_indexCount(0)
        {
        }
    };

    class GLMMDClothingManager
    {
    public:
        GLMMDClothingManager();
        ~GLMMDClothingManager();

        GLMMDClothingManager(const GLMMDClothingManager&) = delete;
        GLMMDClothingManager& operator=(const GLMMDClothingManager&) = delete;

        int AddClothing(int modelIndex, const std::string& clothingPmxPath, const std::string& boneAttachment);
        bool RemoveClothing(int modelIndex, int clothingId);
        bool ToggleClothingVisibility(int modelIndex, int clothingId);
        bool SetClothingVisibility(int modelIndex, int clothingId, bool visible);

        const GLMMDClothingItem* GetClothing(int modelIndex, int clothingId) const;
        GLMMDClothingItem* GetClothing(int modelIndex, int clothingId);

        std::vector<int> GetClothingIds(int modelIndex) const;
        size_t GetClothingCount(int modelIndex) const;

        void ClearModel(int modelIndex);
        void ClearAll();

    private:
        struct ModelClothingData
        {
            std::map<int, std::unique_ptr<GLMMDClothingItem>> m_items;
            int m_nextId = 1;
        };

        using ModelClothingMap = std::map<int, ModelClothingData>;

        bool LoadClothingMesh(GLMMDClothingItem* item, const std::string& pmxPath);

        ModelClothingMap m_clothingData;
    };
}

#endif