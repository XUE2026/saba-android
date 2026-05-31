//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "EnvironmentManager.h"

namespace saba
{
	EnvironmentManager::EnvironmentManager()
		: m_nextPrimitiveId(1)
		, m_nextObjectId(1)
	{
	}

	EnvironmentManager::~EnvironmentManager()
	{
	}

	void EnvironmentManager::SetSun(const SunParams& sun)
	{
		m_sun = sun;
	}

	void EnvironmentManager::SetGround(const GroundParams& ground)
	{
		m_ground = ground;
	}

	int EnvironmentManager::AddPrimitive(PrimitiveType type, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	{
		PrimitiveInfo info;
		info.type = type;
		info.position = position;
		info.rotation = rotation;
		info.scale = scale;
		info.id = m_nextPrimitiveId++;
		m_primitives.push_back(info);
		return info.id;
	}

	bool EnvironmentManager::RemovePrimitive(int id)
	{
		for (auto it = m_primitives.begin(); it != m_primitives.end(); ++it)
		{
			if (it->id == id)
			{
				m_primitives.erase(it);
				return true;
			}
		}
		return false;
	}

	int EnvironmentManager::AddObject(EnvironmentObject type, const glm::vec3& position, const glm::vec3& rotation)
	{
		ObjectInfo info;
		info.type = type;
		info.position = position;
		info.rotation = rotation;
		info.id = m_nextObjectId++;
		m_objects.push_back(info);
		return info.id;
	}

	bool EnvironmentManager::RemoveObject(int id)
	{
		for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
		{
			if (it->id == id)
			{
				m_objects.erase(it);
				return true;
			}
		}
		return false;
	}

	const SunParams& EnvironmentManager::GetSun() const
	{
		return m_sun;
	}

	const GroundParams& EnvironmentManager::GetGround() const
	{
		return m_ground;
	}

	const std::vector<PrimitiveInfo>& EnvironmentManager::GetPrimitives() const
	{
		return m_primitives;
	}

	const std::vector<ObjectInfo>& EnvironmentManager::GetObjects() const
	{
		return m_objects;
	}
}