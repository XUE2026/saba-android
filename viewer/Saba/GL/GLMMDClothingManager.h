//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_GL_GLMMDCLOTHINGMANAGER_H_
#define SABA_GL_GLMMDCLOTHINGMANAGER_H_

#include <Saba/GL/Model/MMD/GLMMDModel.h>

#include <string>
#include <vector>
#include <memory>

namespace saba
{
	class PMXModel;

	struct ClothingInfo
	{
		int index;
		std::string pmxPath;
		std::string texturePath;
		std::string boneName;
		std::shared_ptr<PMXModel> clothingModel;
	};

	class GLMMDClothingManager
	{
	public:
		GLMMDClothingManager();
		~GLMMDClothingManager();

		bool AddClothing(GLMMDModel* model, const std::string& pmxPath);
		bool RemoveClothing(int index);
		bool SetClothingTexture(int index, const std::string& texturePath);
		bool BindToBone(int index, const std::string& boneName);

		size_t GetClothingCount() const;
		const ClothingInfo* GetClothingInfo(int index) const;

	private:
		std::vector<std::unique_ptr<ClothingInfo>> m_clothingItems;
	};
}

#endif // !SABA_GL_GLMMDCLOTHINGMANAGER_H_