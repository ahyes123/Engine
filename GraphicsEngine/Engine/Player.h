#pragma once
#include "Scene/Transform.h"
#include "../External/entt/entt.hpp"

class Player
{
public:
	void Init(const entt::entity& ent);
	void Update();
	entt::entity myEnt;
};