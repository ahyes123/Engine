#pragma once
#include "Camera.h"
#include "Engine/EC/Entity.h"

class Scene
{
private:
	std::shared_ptr<Camera> myMainCamera;
	std::wstring myName;
	std::vector<std::shared_ptr<Entity>> myEntities;

public:

	void AddEntity()
	{
		std::shared_ptr<Entity> entity = std::make_shared<Entity>();
		myEntities.push_back(entity);
	}

	std::vector<std::shared_ptr<Entity>> GetEntities() { return myEntities; }

	template<typename T>
	std::vector<std::shared_ptr<Entity>> GetEntitiesWithComponent()
	{
		std::vector<std::shared_ptr<Entity>> entities;
		for (auto e : myEntities)
		{
			if (e->HasComponent<T>())
			{
				entities.push_back(e);
			}
		}
		return entities;
	}
	template<typename T>
	std::vector<std::shared_ptr<T>> GetComponents()
	{
		std::vector<std::shared_ptr<T>> components;
		for (auto e : myEntities)
		{
			if (e->HasComponent<T>())
			{
				components.push_back(e->GetComponent<T>());
			}
		}
		return components;
	}

	void SetCamera(std::shared_ptr<Camera> aCamera)
	{
		myMainCamera = aCamera;
	}

	std::shared_ptr<Camera> const& GetCamera() const
	{
		return myMainCamera;
	}

	const std::wstring GetSceneName() const { return myName; }

	void SetSceneName(const std::wstring& aSceneName) { myName = aSceneName; }
};
