#include "GraphicsEngine.pch.h"
#include "Entity.h"
#include <random>
#include "Component.h"

Entity::Entity()
{
	myID = GenerateRandomID();
}

void Entity::Update()
{
	for (auto& comp : myComponents) 
	{
		comp->Update();
	}
}

void Entity::AddComponent(std::shared_ptr<Component> aComp)
{
	myComponents.push_back(aComp);
	myComponents.back()->SetEntity(this);
	myComponents.back()->Init();
	myComponentMap.insert({ myComponents.back()->GetName(), myComponents.back() });
}

uint32_t Entity::GenerateRandomID()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<uint32_t> dis;
	return dis(gen);
}
