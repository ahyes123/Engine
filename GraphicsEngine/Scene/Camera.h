#pragma once
#include "Math/Matrix4x4.hpp"
#include "Math/Vector2.hpp"
#include "Transform.h"
#include "SceneObject.h"

class Camera : public SceneObject
{
	Transform myTransform;
	Matrix4x4f myProjection;
	Vector2f myCapturedMousePos;

	float myNearPlane = 0.1f;
	float myFarPlane = 1500.0f;
	float myCameraSpeed = 0.f;
	float myFov = 90.f;

public:
	bool Init(float aHorizontalFOV, CommonUtilities::Vector2<unsigned int> aResolution, float aNearPlane, float aFarPlane);
	void Update(float aDeltaTime);

	FORCEINLINE Matrix4x4f const& GetProjection() const { return myProjection; }
	FORCEINLINE float GetNearPlane() const { return myNearPlane; }
	FORCEINLINE float GetFarPlane() const { return myFarPlane; }
	FORCEINLINE float& GetCameraSpeed() { return myCameraSpeed; }
	FORCEINLINE float GetFOV() const { return myFov; }
};

