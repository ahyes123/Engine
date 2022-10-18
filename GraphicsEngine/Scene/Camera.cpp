#include "GraphicsEngine.pch.h"
#include "Camera.h"
#include <cassert>
#include <cmath>
#include "Tools/InputHandler.h"
#include "Tools/UtilityFunctions.hpp"

using namespace CommonUtilities;

bool Camera::Init(float aHorizontalFOV, CommonUtilities::Vector2<unsigned> aResolution, float aNearPlane,
	float aFarPlane)
{
	assert(aNearPlane < aFarPlane);
	assert(aNearPlane >= 0.000001f);

	myNearPlane = aNearPlane;
	myFarPlane = aFarPlane;
	myFov = aHorizontalFOV;

	const float hFoVRad = myFov * (3.14159265358979323846f / 180.0f);

	const float vFovRad =
		2 * std::atan(std::tan(hFoVRad * 0.5f)
			* (static_cast<float>(aResolution.y) / static_cast<float>(aResolution.x)));

	const float myXScale = 1 / std::tanf(hFoVRad * 0.5f);
	const float myYScale = 1 / std::tanf(vFovRad * 0.5f);
	const float Q = myFarPlane / (myFarPlane - myNearPlane);

	myProjection(1, 1) = myXScale;
	myProjection(2, 2) = myYScale;
	myProjection(3, 3) = Q;
	myProjection(3, 4) = 1 / Q;
	myProjection(4, 3) = -Q * myNearPlane;
	myProjection(4, 4) = 0;

	myCameraSpeed = 5;

	return true;
}

void Camera::Update(float aDeltaTime)
{
	aDeltaTime;
	Vector3f rot = GetRotation();
	rot = { rot.x, rot.y, rot.z };
	Vector3f pos = GetTransform().GetPosition();
	Vector3f movedPos;

	if (InputHandler::GetMouseTwoWasPressed())
	{
		myCapturedMousePos = InputHandler::GetMousePosition();
		InputHandler::CaptureMouse();
		InputHandler::HideMouse();
	}
	if (InputHandler::GetMouseTwoIsHeld())
	{
		if (InputHandler::GetKeyIsHeld('W'))
		{
			movedPos += GetTransform().GetForward();
		}
		if (InputHandler::GetKeyIsHeld('S'))
		{
			movedPos -= GetTransform().GetForward();
		}
		if (InputHandler::GetKeyIsHeld('D'))
		{
			movedPos += GetTransform().GetRight();
		}
		if (InputHandler::GetKeyIsHeld('A'))
		{
			movedPos -= GetTransform().GetRight();
		}
		if (InputHandler::GetKeyIsHeld(VK_SPACE))
		{
			movedPos += GetTransform().GetUp();
		}
		if (InputHandler::GetKeyIsHeld(VK_CONTROL))
		{
			movedPos -= GetTransform().GetUp();
		}
		if (InputHandler::GetKeyIsPressed(VK_SHIFT))
		{
			myCameraSpeed *= 5;
		}
		else if (InputHandler::GetKeyWasReleased(VK_SHIFT))
		{
			myCameraSpeed /= 5;
		}

		movedPos = movedPos.Length() > 0 ? movedPos.GetNormalized() : movedPos;
		SetPosition(movedPos * myCameraSpeed + pos);


		Vector2f deltaPos = InputHandler::GetMousePositionDelta();
		float xRot = rot.x + deltaPos.y * 0.05f;
		float yRot = rot.y + deltaPos.x * 0.05f;
		SetRotation(Clamp(-180.f * 0.5f, 180.f * 0.5f, xRot), yRot, rot.z);
		InputHandler::SetMousePosition(static_cast<int>(myCapturedMousePos.x), static_cast<int>(myCapturedMousePos.y));
	}

	if (InputHandler::GetMouseTwoWasReleased())
	{
		InputHandler::SetMousePosition(static_cast<int>(myCapturedMousePos.x), static_cast<int>(myCapturedMousePos.y));
		InputHandler::ReleaseMouse();
		InputHandler::ShowMouse();
	}
}