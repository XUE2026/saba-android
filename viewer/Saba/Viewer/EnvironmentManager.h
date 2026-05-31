//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef SABA_VIEWER_ENVIRONMENTMANAGER_H_
#define SABA_VIEWER_ENVIRONMENTMANAGER_H_

#include <glm/vec3.hpp>
#include <string>
#include <vector>
#include <memory>

namespace saba
{
	enum class GroundType
	{
		Grid,
		Checker,
		Grass,
		Dirt,
		Asphalt
	};

	enum class PrimitiveType
	{
		Cube,
		Sphere,
		Cone,
		Cylinder
	};

	enum class EnvironmentObject
	{
		Tree,
		Flower,
		Bird,
		Butterfly
	};

	struct SunParams
	{
		glm::vec3 direction = glm::vec3(0.5f, -1.0f, 0.3f);
		glm::vec3 color = glm::vec3(1.0f);
		float intensity = 1.0f;
	};

	struct GroundParams
	{
		GroundType type = GroundType::Grid;
		glm::vec3 color1 = glm::vec3(0.8f);
		glm::vec3 color2 = glm::vec3(0.6f);
		float size = 10.0f;
	};

	struct PrimitiveInfo
	{
		PrimitiveType type;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		int id;
	};

	struct ObjectInfo
	{
		EnvironmentObject type;
		glm::vec3 position;
		glm::vec3 rotation;
		int id;
	};

	class EnvironmentManager
	{
	public:
		EnvironmentManager();
		~EnvironmentManager();

		void SetSun(const SunParams& sun);
		void SetGround(const GroundParams& ground);
		int AddPrimitive(PrimitiveType type, const glm::vec3& position, const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));
		bool RemovePrimitive(int id);
		int AddObject(EnvironmentObject type, const glm::vec3& position, const glm::vec3& rotation = glm::vec3(0.0f));
		bool RemoveObject(int id);

		const SunParams& GetSun() const;
		const GroundParams& GetGround() const;
		const std::vector<PrimitiveInfo>& GetPrimitives() const;
		const std::vector<ObjectInfo>& GetObjects() const;

	private:
		SunParams m_sun;
		GroundParams m_ground;
		std::vector<PrimitiveInfo> m_primitives;
		std::vector<ObjectInfo> m_objects;
		int m_nextPrimitiveId;
		int m_nextObjectId;
	};
}

#endif // !SABA_VIEWER_ENVIRONMENTMANAGER_H_