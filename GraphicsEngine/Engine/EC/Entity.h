#include "Transform.h"

#pragma once
class Component;
class Entity
{
public:
	Entity();

	void Update();

	void AddComponent(std::shared_ptr<Component> aComp);
	std::vector<std::shared_ptr<Component>> GetComponents() { return myComponents; }

	std::string& GetName() { return myName; }

	const uint32_t GetID() const { return myID; }

	Transform& GetTransform() { return myTransform; }

	template<class T> bool HasComponent();
	template<class T> std::shared_ptr<T> GetComponent();

private:
	uint32_t GenerateRandomID();

	uint32_t myID;

	std::vector<std::shared_ptr<Component>> myComponents;
	std::unordered_map<std::string, std::shared_ptr<Component>> myComponentMap;

	std::string myName = "Entity";

	Transform myTransform;
};

template<class T>
inline bool Entity::HasComponent()
{
	auto it = myComponentMap.find(T::GetFactoryName());
	if (it != myComponentMap.end())
	{
		return true;
	}
	return false;
}

template<class T>
inline std::shared_ptr<T> Entity::GetComponent()
{
	auto it = myComponentMap.find(T::GetFactoryName());
	if (it != myComponentMap.end())
	{
		return std::reinterpret_pointer_cast<T>(it->second);
	}
	return nullptr;
}