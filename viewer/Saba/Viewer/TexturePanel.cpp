//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "TexturePanel.h"

#include "../GL/Model/MMD/GLMMDModelDrawer.h"

#include <Saba/Base/Log.h>
#include <Saba/Base/Path.h>
#include <Saba/GL/GLTextureUtil.h>
#include <Saba/Model/MMD/MMDNode.h>

#include <imgui.h>

namespace saba
{
    TexturePanel::TexturePanel()
        : m_visible(false)
        , m_currentDrawer(nullptr)
        , m_selectedMaterialIndex(-1)
        , m_selectedClothingIndex(-1)
    {
    }

    TexturePanel::~TexturePanel()
    {
    }

    void TexturePanel::SetTextureManager(std::shared_ptr<TextureManager> texMan)
    {
        m_textureManager = texMan;
    }

    void TexturePanel::SetClothingManager(std::shared_ptr<GLMMDClothingManager> clothingMan)
    {
        m_clothingManager = clothingMan;
    }

    void TexturePanel::SetCurrentModelDrawer(GLMMDModelDrawer* drawer)
    {
        m_currentDrawer = drawer;
        m_selectedMaterialIndex = -1;
        m_selectedClothingIndex = -1;
        m_texturePathInput.clear();
        m_clothingPathInput.clear();
        m_boneAttachmentInput.clear();
    }

    void TexturePanel::Draw()
    {
        if (!m_visible)
        {
            return;
        }

        ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Texture & Clothing Panel", &m_visible))
        {
            ImGui::End();
            return;
        }

        if (m_currentDrawer == nullptr)
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "No MMD model selected.");
            ImGui::Text("Select an MMD model in the scene to edit its textures.");
            ImGui::End();
            return;
        }

        GLMMDModel* model = m_currentDrawer->GetModel();
        if (model == nullptr)
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Model data is null.");
            ImGui::End();
            return;
        }

        if (ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen))
        {
            DrawMaterialSection();
        }

        if (ImGui::CollapsingHeader("Clothing", ImGuiTreeNodeFlags_DefaultOpen))
        {
            DrawClothingSection();
        }

        ImGui::End();
    }

    void TexturePanel::DrawMaterialSection()
    {
        GLMMDModel* model = m_currentDrawer->GetModel();
        MMDModel* mmdModel = model->GetMMDModel();
        if (mmdModel == nullptr)
        {
            return;
        }

        size_t matCount = mmdModel->GetMaterialCount();
        if (matCount == 0)
        {
            ImGui::Text("No materials found.");
            return;
        }

        ImGui::Text("Materials (%zu):", matCount);
        ImGui::Separator();

        ImGui::BeginChild("MaterialList", ImVec2(0, 250), true);

        for (size_t i = 0; i < matCount; i++)
        {
            const auto& mmdMat = mmdModel->GetMaterials()[i];
            const auto& glMat = model->GetMaterials()[i];

            std::string matLabel = "Material " + std::to_string(i);
            if (!mmdMat.m_texture.empty())
            {
                matLabel += " [" + PathUtil::GetFilename(mmdMat.m_texture) + "]";
            }
            else
            {
                matLabel += " [no tex]";
            }

            ImGui::PushID(static_cast<int>(i));

            bool hasReplacement = false;
            if (m_textureManager)
            {
                hasReplacement = m_textureManager->HasReplacement(0, mmdMat.m_texture);
            }

            if (hasReplacement)
            {
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "%s", matLabel.c_str());
            }
            else
            {
                ImGui::Text("%s", matLabel.c_str());
            }

            ImGui::SameLine();
            if (ImGui::SmallButton("Replace"))
            {
                m_selectedMaterialIndex = static_cast<int>(i);
                m_texturePathInput = mmdMat.m_texture;

                ImGui::OpenPopup("ReplaceTexturePopup");
            }

            ImGui::SameLine();
            if (hasReplacement)
            {
                if (ImGui::SmallButton("Reset"))
                {
                    if (m_textureManager)
                    {
                        m_textureManager->ResetTexture(0, mmdMat.m_texture);
                        ResetTextureReplacement(mmdMat.m_texture);
                    }
                }
            }

            if (ImGui::IsItemHovered() && !mmdMat.m_texture.empty())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Original: %s", mmdMat.m_texture.c_str());
                if (hasReplacement && m_textureManager)
                {
                    std::string replacementPath = m_textureManager->GetReplacementPath(0, mmdMat.m_texture);
                    if (!replacementPath.empty())
                    {
                        ImGui::Text("Replacement: %s", replacementPath.c_str());
                    }
                }
                ImGui::EndTooltip();
            }

            ImGui::Separator();
            ImGui::PopID();
        }

        ImGui::EndChild();

        if (ImGui::BeginPopup("ReplaceTexturePopup"))
        {
            ImGui::Text("Replace Texture for Material %d", m_selectedMaterialIndex);
            ImGui::Separator();

            if (m_selectedMaterialIndex >= 0 && m_selectedMaterialIndex < static_cast<int>(matCount))
            {
                const auto& mmdMat = mmdModel->GetMaterials()[m_selectedMaterialIndex];
                ImGui::Text("Current: %s", mmdMat.m_texture.c_str());
            }

            char buf[512];
            std::strncpy(buf, m_texturePathInput.c_str(), sizeof(buf) - 1);
            buf[sizeof(buf) - 1] = '\0';

            if (ImGui::InputText("Texture Path", buf, sizeof(buf)))
            {
                m_texturePathInput = buf;
            }

            if (ImGui::Button("Apply", ImVec2(120, 0)))
            {
                if (!m_texturePathInput.empty() && m_selectedMaterialIndex >= 0)
                {
                    const auto& mmdMat = mmdModel->GetMaterials()[m_selectedMaterialIndex];
                    if (m_textureManager)
                    {
                        m_textureManager->ReplaceTexture(0, mmdMat.m_texture, m_texturePathInput);
                    }
                    ApplyTextureReplacement(mmdMat.m_texture, m_texturePathInput);
                }
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void TexturePanel::DrawClothingSection()
    {
        if (m_clothingManager == nullptr)
        {
            ImGui::Text("Clothing manager not available.");
            return;
        }

        if (ImGui::CollapsingHeader("Add New Clothing", ImGuiTreeNodeFlags_DefaultOpen))
        {
            char pathBuf[512];
            std::strncpy(pathBuf, m_clothingPathInput.c_str(), sizeof(pathBuf) - 1);
            pathBuf[sizeof(pathBuf) - 1] = '\0';

            ImGui::InputText("PMX Path", pathBuf, sizeof(pathBuf));
            m_clothingPathInput = pathBuf;

            char boneBuf[128];
            std::strncpy(boneBuf, m_boneAttachmentInput.c_str(), sizeof(boneBuf) - 1);
            boneBuf[sizeof(boneBuf) - 1] = '\0';

            ImGui::InputText("Bone Attachment", boneBuf, sizeof(boneBuf));
            ImGui::SameLine();
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Name of the bone this clothing should follow");
            }
            m_boneAttachmentInput = boneBuf;

            if (ImGui::Button("Add Clothing", ImVec2(200, 0)))
            {
                if (!m_clothingPathInput.empty())
                {
                    int clothingId = m_clothingManager->AddClothing(
                        0,
                        m_clothingPathInput,
                        m_boneAttachmentInput
                    );
                    if (clothingId >= 0)
                    {
                        SABA_INFO("TexturePanel: Added clothing [{}] with id [{}]",
                            m_clothingPathInput, clothingId);
                        m_clothingPathInput.clear();
                        m_boneAttachmentInput.clear();
                    }
                }
            }
        }

        size_t clothingCount = m_clothingManager->GetClothingCount(0);
        if (clothingCount > 0)
        {
            ImGui::Separator();
            ImGui::Text("Added Clothing (%zu):", clothingCount);

            ImGui::BeginChild("ClothingList", ImVec2(0, 150), true);

            std::vector<int> clothingIds = m_clothingManager->GetClothingIds(0);
            for (int clothingId : clothingIds)
            {
                const GLMMDClothingItem* item = m_clothingManager->GetClothing(0, clothingId);
                if (item == nullptr)
                {
                    continue;
                }

                ImGui::PushID(clothingId);

                bool visible = item->m_visible;
                if (ImGui::Checkbox("##vis", &visible))
                {
                    m_clothingManager->SetClothingVisibility(0, clothingId, visible);
                }

                ImGui::SameLine();
                ImGui::Text("[%d] %s", clothingId, item->m_name.c_str());

                if (!item->m_boneAttachment.empty())
                {
                    ImGui::SameLine();
                    ImGui::TextDisabled("-> %s", item->m_boneAttachment.c_str());
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("Remove"))
                {
                    m_clothingManager->RemoveClothing(0, clothingId);
                }

                ImGui::PopID();
            }

            ImGui::EndChild();
        }
    }

    void TexturePanel::ApplyTextureReplacement(const std::string& materialName, const std::string& newTexturePath)
    {
        GLMMDModel* model = m_currentDrawer->GetModel();
        MMDModel* mmdModel = model->GetMMDModel();
        if (mmdModel == nullptr)
        {
            return;
        }

        size_t matCount = mmdModel->GetMaterialCount();
        for (size_t i = 0; i < matCount; i++)
        {
            const auto& mmdMat = mmdModel->GetMaterials()[i];
            if (mmdMat.m_texture == materialName)
            {
                GLTextureObject newTex = CreateTextureFromFile(newTexturePath.c_str(), true, true);
                if (newTex.Get() == 0)
                {
                    SABA_ERROR("TexturePanel: Failed to load texture [{}]", newTexturePath);
                    return;
                }

                auto& materials = const_cast<std::vector<GLMMDMaterial>&>(model->GetMaterials());
                materials[i].m_texture = std::move(newTex);
                materials[i].m_textureHaveAlpha = IsAlphaTexture(materials[i].m_texture);

                SABA_INFO("TexturePanel: Applied texture replacement for material [{}] -> [{}]",
                    materialName, newTexturePath);
                return;
            }
        }

        SABA_WARN("TexturePanel: Material [{}] not found for texture replacement", materialName);
    }

    void TexturePanel::ResetTextureReplacement(const std::string& materialName)
    {
        GLMMDModel* model = m_currentDrawer->GetModel();
        MMDModel* mmdModel = model->GetMMDModel();
        if (mmdModel == nullptr)
        {
            return;
        }

        size_t matCount = mmdModel->GetMaterialCount();
        for (size_t i = 0; i < matCount; i++)
        {
            const auto& mmdMat = mmdModel->GetMaterials()[i];
            if (mmdMat.m_texture == materialName)
            {
                std::string originalPath = mmdMat.m_texture;
                if (originalPath.empty())
                {
                    auto& materials = const_cast<std::vector<GLMMDMaterial>&>(model->GetMaterials());
                    materials[i].m_texture.Release();
                    materials[i].m_textureHaveAlpha = false;
                    SABA_INFO("TexturePanel: Reset texture for material [{}] (no original texture)", materialName);
                }
                else
                {
                    GLTextureObject originalTex = CreateTextureFromFile(originalPath.c_str(), true, true);
                    auto& materials = const_cast<std::vector<GLMMDMaterial>&>(model->GetMaterials());
                    materials[i].m_texture = std::move(originalTex);
                    materials[i].m_textureHaveAlpha = IsAlphaTexture(materials[i].m_texture);
                    SABA_INFO("TexturePanel: Reset texture for material [{}] -> original [{}]",
                        materialName, originalPath);
                }
                return;
            }
        }
    }
}