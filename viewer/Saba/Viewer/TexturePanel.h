//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_TEXTUREPANEL_H_
#define SABA_VIEWER_TEXTUREPANEL_H_

#include <memory>
#include <vector>
#include <string>

#include <Saba/Model/MMD/TextureManager.h>
#include "../GL/Model/MMD/GLMMDClothingManager.h"
#include "../GL/Model/MMD/GLMMDModel.h"

namespace saba
{
    class GLMMDModelDrawer;

    class TexturePanel
    {
    public:
        TexturePanel();
        ~TexturePanel();

        TexturePanel(const TexturePanel&) = delete;
        TexturePanel& operator=(const TexturePanel&) = delete;

        void SetTextureManager(std::shared_ptr<TextureManager> texMan);
        void SetClothingManager(std::shared_ptr<GLMMDClothingManager> clothingMan);

        void SetCurrentModelDrawer(GLMMDModelDrawer* drawer);

        void Draw();
        bool IsVisible() const { return m_visible; }
        void SetVisible(bool visible) { m_visible = visible; }
        void ToggleVisible() { m_visible = !m_visible; }

    private:
        void DrawMaterialSection();
        void DrawClothingSection();

        void ApplyTextureReplacement(const std::string& materialName, const std::string& newTexturePath);
        void ResetTextureReplacement(const std::string& materialName);

        bool m_visible;

        std::shared_ptr<TextureManager>         m_textureManager;
        std::shared_ptr<GLMMDClothingManager>   m_clothingManager;
        GLMMDModelDrawer*                       m_currentDrawer;

        std::string m_texturePathInput;
        std::string m_clothingPathInput;
        std::string m_boneAttachmentInput;
        int         m_selectedMaterialIndex;
        int         m_selectedClothingIndex;
    };
}

#endif