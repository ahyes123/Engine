#include "../GraphicsEngine.pch.h"
#include "GraphicsEngine.pch.h"
#include "ComponentHandler.h"
#include "../Scene/SceneHandler.h"
#include "../Tools/Timer.h"

std::vector<std::string> ComponentHandler::myComponents;

void ComponentHandler::Init()
{
	myComponents.push_back("ModelComponent");
	myComponents.push_back("PlayerComponent");
	myComponents.push_back("TransformComponent");
	myComponents.push_back("ParticleComponent");
	myComponents.push_back("TextComponent");
}

void ComponentHandler::Update()
{
	entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
	auto player = reg.view<PlayerComponent>();
	for (auto entity : player) {
		player.get<PlayerComponent>(entity).myPlayer.Update();
	}
	auto particleSystems = reg.view<ParticleSystemComponent>();
	for (auto entity : particleSystems) {
		std::shared_ptr<ParticleSystem> sys = particleSystems.get<ParticleSystemComponent>(entity).myParticleSystem;
		particleSystems.get<ParticleSystemComponent>(entity).myParticleSystem->Update(CommonUtilities::Timer::GetDeltaTime());
	}
}

void ComponentHandler::AddComponent(const int& aComponent, entt::entity& aEntity)
{
	entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
	switch (aComponent)
	{
	case 0:
		if (reg.any_of<ModelComponent>(aEntity))
			return;
		reg.emplace<ModelComponent>(aEntity);
		reg.get<ModelComponent>(aEntity).myModel = std::make_shared<ModelInstance>();
		break;
	case 1:
		if (reg.any_of<PlayerComponent>(aEntity))
			return;
		reg.emplace<PlayerComponent>(aEntity);
		reg.get<PlayerComponent>(aEntity).myPlayer.Init(aEntity);
		break;
	case 2:
		if (reg.any_of<TransformComponent>(aEntity))
			return;
		reg.emplace<TransformComponent>(aEntity);
		break;
	case 3:
		if (reg.any_of<ParticleSystemComponent>(aEntity))
			return;
		reg.emplace<ParticleSystemComponent>(aEntity);
		break;
	case 4:
		if (reg.any_of<TextComponent>(aEntity))
			return;
		reg.emplace<TextComponent>(aEntity);
		break;
	}
}

std::vector<int> ComponentHandler::GetAllComponentIDs(const entt::entity& aEntity)
{
	entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
	std::vector<int> ids;
	if (reg.any_of<ModelComponent>(aEntity))
		ids.push_back(0);
	if (reg.any_of<PlayerComponent>(aEntity))
		ids.push_back(1);
	if (reg.any_of<TransformComponent>(aEntity))
		ids.push_back(2);
	if (reg.any_of<ParticleSystemComponent>(aEntity))
		ids.push_back(3);
	if (reg.any_of<TextComponent>(aEntity))
		ids.push_back(4);
	return ids;
}

entt::entity& ComponentHandler::DuplicateEntity(const entt::entity aEntity)
{
	entt::registry& reg = SceneHandler::GetActiveScene()->GetRegistry();
	std::vector<int> ids = GetAllComponentIDs(aEntity);
	entt::entity entity = reg.create();
	for (size_t i = 0; i < ids.size(); i++)
	{
		AddComponent(ids[i], entity);
	}
	if (reg.all_of<ModelComponent>(aEntity))
		reg.get<ModelComponent>(entity).myModel = reg.get<ModelComponent>(aEntity).myModel;
	if (reg.all_of<PlayerComponent>(aEntity))
	{
		reg.get<PlayerComponent>(entity).myPlayer = reg.get<PlayerComponent>(aEntity).myPlayer;
		reg.get<PlayerComponent>(entity).myPlayer.Init(entity);
	}
	if (reg.all_of<TransformComponent>(aEntity))
		reg.get<TransformComponent>(entity).myTransform = reg.get<TransformComponent>(aEntity).myTransform;

	if (reg.all_of<ParticleSystemComponent>(aEntity))
		reg.get<ParticleSystemComponent>(entity).myParticleSystem = reg.get<ParticleSystemComponent>(aEntity).myParticleSystem;
	if (reg.all_of<TextComponent>(aEntity))
		reg.get<TextComponent>(entity).myText = reg.get<TextComponent>(aEntity).myText;
	std::shared_ptr<entt::entity> ent = std::make_shared<entt::entity>(entity);
	return *ent;
}

template<typename T>
void ComponentHandler::AddComponent(entt::entity& aEntity)
{
	SceneHandler::GetActiveScene()->GetRegistry().emplace<T>(aEntity);
}