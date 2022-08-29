#include "GraphicsEngine.pch.h"
#include "Player.h"
#include "../Scene/SceneHandler.h"
#include "../Model/ModelInstance.h"
#include "ComponentHandler.h"


void Player::Init(const entt::entity& ent)
{
	myEnt = ent;
}

void Player::Update()
{
	Transform& transform = SceneHandler::GetActiveScene()->GetRegistry().get<TransformComponent>(myEnt).myTransform;
	if (CommonUtilities::InputHandler::GetKeyIsHeld('J'))
		transform.SetPosition({ transform.GetPosition().x - 1, transform.GetPosition().y, transform.GetPosition().z });
	if (CommonUtilities::InputHandler::GetKeyIsHeld('K'))
		transform.SetPosition({ transform.GetPosition().x, transform.GetPosition().y - 1, transform.GetPosition().z });
	if (CommonUtilities::InputHandler::GetKeyIsHeld('L'))
		transform.SetPosition({ transform.GetPosition().x + 1, transform.GetPosition().y, transform.GetPosition().z });
	if (CommonUtilities::InputHandler::GetKeyIsHeld('I'))
		transform.SetPosition({ transform.GetPosition().x, transform.GetPosition().y + 1, transform.GetPosition().z });
}