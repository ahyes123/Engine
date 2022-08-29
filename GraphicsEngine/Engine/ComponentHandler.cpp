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

template<typename T>
void ComponentHandler::AddComponent(entt::entity& aEntity)
{
	SceneHandler::GetActiveScene()->GetRegistry().emplace<T>(aEntity);
}