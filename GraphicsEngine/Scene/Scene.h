#pragma once
#include "Camera.h"
#include "Model/Model.h"
#include "Model/ModelInstance.h"
#include "InputHandler.h"
#include "Particle/ParticleSystem.h"
#include "Text/Text.h"
#include "../External/entt/entity/entity.hpp"
#include "../External/entt/entity/registry.hpp"
#include "../Engine/ComponentHandler.h"

enum class ObjectType
{
	Model,
	ParticleSystem,
	Text,
	All
};
class Scene
{
	struct ModelObject
	{
		std::vector<std::shared_ptr<ModelInstance>> myModels;
		std::vector<entt::entity> myEntitys;
	};
	struct ParticleSystemObject
	{
		std::vector<std::shared_ptr<ParticleSystem>> mySystems;
		std::vector<entt::entity> myEntitys;
	};
	struct TextObject
	{
		std::vector<std::shared_ptr<Text>> myTexts;
		std::vector<entt::entity> myEntitys;
	};

	struct Objects
	{
		std::vector<std::shared_ptr<SceneObject>> mySceneObjects;
		std::vector<entt::entity> myEntitys;
	};

	ModelObject myModelObject;
	ParticleSystemObject myParticleSystems;
	TextObject myTextObject;
	Objects myObjects;
	std::shared_ptr<Camera> myMainCamera;
	std::wstring myName;
	entt::registry myRegistry;

public:
	entt::registry& GetRegistry()
	{
		return myRegistry;
	}

	std::shared_ptr<SceneObject> GetObjectByID(const int& aId)
	{
		for (size_t i = 0; i < myObjects.mySceneObjects.size(); i++)
		{
			if (myObjects.mySceneObjects[i]->GetId() == aId)
			{
				return myObjects.mySceneObjects[i];
			}
		}
		return nullptr;
	}

	std::vector<entt::entity>& GetEntitys(const ObjectType& aEntityType = ObjectType::Model)
	{
		switch (aEntityType)
		{
		case ObjectType::Model:
			return myModelObject.myEntitys;
			break;
		case ObjectType::ParticleSystem:
			return myParticleSystems.myEntitys;
			break;
		case ObjectType::Text:
			return myTextObject.myEntitys;
			break;
		case ObjectType::All:
			return myObjects.myEntitys;
			break;
		default:
			return myObjects.myEntitys;
			break;
		}
	}

	std::vector<std::shared_ptr<SceneObject>> GetSceneObjects()
	{
		std::vector<std::shared_ptr<SceneObject>> obj;
		for (size_t i = 0; i < myObjects.mySceneObjects.size(); i++)
		{
			if (myObjects.mySceneObjects[i]->myParent == nullptr)
			{
				obj.push_back(myObjects.mySceneObjects[i]);
			}
		}
		return obj;
	}

	std::vector<std::shared_ptr<SceneObject>> GetAllSceneObjects()
	{
		return myObjects.mySceneObjects;
	}

	int GetNextId() 
	{
		int random = rand() % INT_MAX;
		auto obj = GetObjectByID(random);

		while (obj)
		{
			random = rand() % INT_MAX;
			obj = GetObjectByID(random);
		}
		return random;
	}

	void AddModelInstance(std::shared_ptr<ModelInstance> aModelInstance, entt::entity& aEntity)
	{
		myModelObject.myEntitys.push_back(aEntity);
		myModelObject.myModels.push_back(aModelInstance);
		myObjects.myEntitys.push_back(aEntity);
		myObjects.mySceneObjects.push_back(aModelInstance);
		if (!myRegistry.any_of<TransformComponent>(aEntity))
		{
			myRegistry.emplace<TransformComponent>(aEntity);
			aModelInstance->SetTransform(myRegistry.get<TransformComponent>(aEntity).myTransform);
		}
		if (!myRegistry.any_of<ModelComponent>(aEntity))
		{
			myRegistry.emplace<ModelComponent>(aEntity);
			myRegistry.get<ModelComponent>(aEntity).myModel = aModelInstance;
		}
		myObjects.mySceneObjects.back()->myEntity = aEntity;
	}

	void AddText(std::shared_ptr<Text> aText, entt::entity& aEntity)
	{
		myTextObject.myEntitys.push_back(aEntity);
		myTextObject.myTexts.push_back(aText);
		myObjects.myEntitys.push_back(aEntity);
		myObjects.mySceneObjects.push_back(aText);
		myObjects.mySceneObjects.back()->myEntity = aEntity;
		if (!myRegistry.any_of<TransformComponent>(aEntity))
		{
			myRegistry.emplace<TransformComponent>(aEntity);
		}
		if (!myRegistry.any_of<TextComponent>(aEntity))
		{
			myRegistry.emplace<TextComponent>(aEntity);
		}
		myRegistry.get<TextComponent>(aEntity).myText = aText;
	}

	void AddParticleSystem(std::shared_ptr<ParticleSystem> aSystem, entt::entity& aEntity)
	{
		myParticleSystems.myEntitys.push_back(aEntity);
		myParticleSystems.mySystems.push_back(aSystem);
		myObjects.myEntitys.push_back(aEntity);
		myObjects.mySceneObjects.push_back(aSystem);
		myObjects.mySceneObjects.back()->myEntity = aEntity;
		if (!myRegistry.any_of<TransformComponent>(aEntity))
		{
			myRegistry.emplace<TransformComponent>(aEntity);
		}
		if (!myRegistry.any_of<ParticleSystemComponent>(aEntity))
		{
			myRegistry.emplace<ParticleSystemComponent>(aEntity);
		}
		myRegistry.get<ParticleSystemComponent>(aEntity).myParticleSystem = aSystem;
	}

	void RemoveModelInstance(std::shared_ptr<ModelInstance> aModelInstance)
	{
		for (size_t i = 0; i < myObjects.mySceneObjects.size(); i++)
		{
			if (myModelObject.myModels.size() > i && myModelObject.myModels[i] != nullptr)
			{
				if (myModelObject.myModels[i] == aModelInstance)
				{
					myModelObject.myModels.erase(myModelObject.myModels.begin() + i);
					myModelObject.myEntitys.erase(myModelObject.myEntitys.begin() + i);
				}
			}
			if (myObjects.mySceneObjects[i] == aModelInstance)
			{
				myObjects.mySceneObjects.erase(myObjects.mySceneObjects.begin() + i);
				myObjects.myEntitys.erase(myObjects.myEntitys.begin() + i);
			}
		}
	}

	void RemoveText(std::shared_ptr<Text> aText)
	{
		for (size_t i = 0; i < myObjects.mySceneObjects.size(); i++)
		{
			if (myTextObject.myTexts.size() > i && myTextObject.myTexts[i] != nullptr)
			{
				if (myTextObject.myTexts[i] == aText)
				{
					myTextObject.myTexts.erase(myTextObject.myTexts.begin() + i);
					myTextObject.myEntitys.erase(myTextObject.myEntitys.begin() + i);
				}
			}
			if (myObjects.mySceneObjects[i] == aText)
			{
				myObjects.mySceneObjects.erase(myObjects.mySceneObjects.begin() + i);
				myObjects.myEntitys.erase(myObjects.myEntitys.begin() + i);
			}
		}
	}

	void RemoveParticleSystem(std::shared_ptr<ParticleSystem> aSystem)
	{
		for (size_t i = 0; i < myObjects.mySceneObjects.size(); i++)
		{
			if (myParticleSystems.mySystems.size() > i && myParticleSystems.mySystems[i] != nullptr)
			{
				if (myParticleSystems.mySystems[i] == aSystem)
				{
					myParticleSystems.mySystems.erase(myParticleSystems.mySystems.begin() + i);
					myParticleSystems.myEntitys.erase(myParticleSystems.myEntitys.begin() + i);
				}
			}
			if (myObjects.mySceneObjects[i] == aSystem)
			{
				myObjects.mySceneObjects.erase(myObjects.mySceneObjects.begin() + i);
				myObjects.myEntitys.erase(myObjects.myEntitys.begin() + i);
			}
		}
	}

	void RemoveAllGameObjects()
	{
		myModelObject.myModels.clear();
		myModelObject.myEntitys.clear();
		myParticleSystems.mySystems.clear();
		myParticleSystems.myEntitys.clear();
		myTextObject.myTexts.clear();
		myTextObject.myEntitys.clear();
		myObjects.mySceneObjects.clear();
		myObjects.myEntitys.clear();
		myRegistry.clear();
	}

	void SetCamera(std::shared_ptr<Camera> aCamera)
	{
		myMainCamera = aCamera;
	}

	std::vector<std::shared_ptr<ModelInstance>> const& GetModels() const
	{
		return myModelObject.myModels;
	}

	std::vector<std::shared_ptr<Text>> const& GetTexts() const
	{
		return myTextObject.myTexts;
	}

	std::vector<std::shared_ptr<ParticleSystem>> const& GetParticleSystems() const
	{
		return myParticleSystems.mySystems;
	}

	std::shared_ptr<Camera> const& GetCamera() const
	{
		return myMainCamera;
	}

	std::shared_ptr<ModelInstance> GetModel(const std::wstring& aName)
	{
		for (auto& object : myModelObject.myModels)
		{
			if (object->GetName() == aName)
			{
				return object;
			}
		}
		return nullptr;
	}

	void Update(float aDeltaTime)
	{
		for (size_t i = 0; i < myModelObject.myModels.size(); i++)
		{
			auto objects = myModelObject.myModels[i];
			objects->Update(aDeltaTime);
			if (myRegistry.any_of<TransformComponent>(myModelObject.myEntitys[i]))
			{
				Transform& transform = myRegistry.get<TransformComponent>(myModelObject.myEntitys[i]).myTransform;
				//transform.SetRotation({ transform.GetRotation().x,
				//	transform.GetRotation().y + aDeltaTime * 10.f,
				//	transform.GetRotation().z });
				objects->GetTransform() = transform;
			}
		}
		for (size_t i = 0; i < myTextObject.myTexts.size(); i++)
		{
			auto objects = myTextObject.myTexts[i];
			if (myRegistry.any_of<TransformComponent>(myTextObject.myEntitys[i]))
			{
				Transform& transform = myRegistry.get<TransformComponent>(myTextObject.myEntitys[i]).myTransform;
				objects->GetTransform() = transform;
			}
		}
		for (size_t i = 0; i < myParticleSystems.mySystems.size(); i++)
		{
			auto objects = myParticleSystems.mySystems[i];
			if (myRegistry.any_of<TransformComponent>(myParticleSystems.myEntitys[i]))
			{
				Transform& transform = myRegistry.get<TransformComponent>(myParticleSystems.myEntitys[i]).myTransform;
				objects->GetTransform() = transform;
			}
		}
	}

	const std::wstring GetSceneName() const { return myName; }

	void SetSceneName(const std::wstring& aSceneName) { myName = aSceneName; }
};
