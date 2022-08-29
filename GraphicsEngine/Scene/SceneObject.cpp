#include "GraphicsEngine.pch.h"
#include "SceneObject.h"

void SceneObject::SetTransform(const Transform& aTransform)
{
	myTransform = aTransform;
}

void SceneObject::SetRotation(float aPitch, float aYaw, float aRoll)
{
	myTransform.SetRotation(Vector3f(aPitch, aYaw, aRoll));
}

void SceneObject::SetRotation(Vector3f aRot)
{
	myTransform.SetRotation(aRot);
}

void SceneObject::SetPosition(float x, float y, float z)
{
	myTransform.SetPosition(Vector3f(x, y, z));
}

void SceneObject::SetPosition(Vector3f aPos)
{
	myTransform.SetPosition(aPos);
}

void SceneObject::SetScale(float x, float y, float z)
{
	myTransform.SetScale(Vector3f(x, y, z));
}

void SceneObject::SetScale(Vector3f aScale)
{
	myTransform.SetScale(aScale);
}

Vector3f SceneObject::GetRotation() const
{
	return myTransform.GetRotation();
}