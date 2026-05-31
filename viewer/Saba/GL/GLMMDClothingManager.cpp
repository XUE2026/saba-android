//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "GLMMDClothingManager.h"

#include <Saba/Model/MMD/PMXFile.h>
#include <Saba/Model/MMD/PMXModel.h>
#include <Saba/Model/MMD/MMDNode.h>

#include <GL/gl3w.h>

namespace saba
{
	GLMMDClothingManager::GLMMDClothingManager()
	{
	}

	GLMMDClothingManager::~GLMMDClothingManager()
	{
	}

	bool GLMMDClothingManager::AddClothing(GLMMDModel* model, const std::string& pmxPath)
	{
		if (model == nullptr)
		{
			return false;
		}

		MMDModel* mmdModel = model->GetMMDModel();
		if (mmdModel == nullptr)
		{
			return false;
		}

		PMXFile pmxFile;
		if (!pmxFile.Load(pmxPath))
		{
			return false;
		}

		auto clothingModel = std::make_shared<PMXModel>();
		if (!clothingModel->Create(std::move(pmxFile)))
		{
			return false;
		}

		auto info = std::make_unique<ClothingInfo>();
		info->index = static_cast<int>(m_clothingItems.size());
		info->pmxPath = pmxPath;
		info->clothingModel = clothingModel;

		m_clothingItems.push_back(std::move(info));
		return true;
	}

	bool GLMMDClothingManager::RemoveClothing(int index)
	{
		if (index < 0 || static_cast<size_t>(index) >= m_clothingItems.size())
		{
			return false;
		}

		m_clothingItems.erase(m_clothingItems.begin() + index);

		for (size_t i = static_cast<size_t>(index); i < m_clothingItems.size(); i++)
		{
			m_clothingItems[i]->index = static_cast<int>(i);
		}

		return true;
	}

	bool GLMMDClothingManager::SetClothingTexture(int index, const std::string& texturePath)
	{
		if (index < 0 || static_cast<size_t>(index) >= m_clothingItems.size())
		{
			return false;
		}

		m_clothingItems[index]->texturePath = texturePath;
		return true;
	}

	bool GLMMDClothingManager::BindToBone(int index, const std::string& boneName)
	{
		if (index < 0 || static_cast<size_t>(index) >= m_clothingItems.size())
		{
			return false;
		}

		m_clothingItems[index]->boneName = boneName;
		return true;
	}

	size_t GLMMDClothingManager::GetClothingCount() const
	{
		return m_clothingItems.size();
	}

	const ClothingInfo* GLMMDClothingManager::GetClothingInfo(int index) const
	{
		if (index < 0 || static_cast<size_t>(index) >= m_clothingItems.size())
		{
			return nullptr;
		}
		return m_clothingItems[index].get();
	}
}