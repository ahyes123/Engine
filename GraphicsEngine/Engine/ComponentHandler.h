#pragma once
#include "../External/entt/entity/entity.hpp"
#include "../Model/ModelInstance.h"
#include "../Engine/Player.h"
#include "../Scene/Transform.h"
#include "../Particle/ParticleSystem.h"
#include "../Text/Text.h"

struct ModelComponent
{
	std::shared_ptr<ModelInstance> myModel;
};

struct PlayerComponent
{
	Player myPlayer;
};

struct TransformComponent
{
	Transform myTransform;
};

struct ParticleSystemComponent
{
	std::shared_ptr<ParticleSystem> myParticleSystem;
};

struct TextComponent
{
	std::shared_ptr<Text> myText;
};

class ComponentHandler
{
public:
	ComponentHandler() = default;
	~ComponentHandler() = default;

	static void Init();

	template<typename T>
	static void AddComponent(entt::entity& aEntity);

	static void Update();

	static void AddComponent(const int& aComponent, entt::entity& aEntity);

	static std::vector<int> GetAllComponentIDs(const entt::entity& aEntity);

	static const std::vector<std::string> GetComponentNames() { return myComponents; }

	static entt::entity& DuplicateEntity(const entt::entity aEntity);
private:
	static std::vector<std::string> myComponents;
};