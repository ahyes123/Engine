#include "GraphicsEngine.pch.h"
#include "Transform.h"
#include <math.h>

using namespace CommonUtilities;

#define DEG2RAD(a) (a * PI / 180.0f)

Transform::Transform(Matrix4x4f aMatrix)
{
	myMatrix = aMatrix;
	myMatrix.Decompose(myPosition, myRotation, myScale);
	ComposeMatrix();
}

void Transform::SetPosition(const Vector3f& position)
{
	myPosition = position;
	ComposeMatrix();
}

void Transform::SetRotation(const Vector3f& rotation)
{
	myRotation = rotation;
	ComposeMatrix();
}

void Transform::SetScale(const Vector3f& scale)
{
	myScale = scale;
	ComposeMatrix();
}

void Transform::ComposeMatrix(bool aIsAnim)
{
	Matrix4x4f matrix;

	matrix(1, 1) = myScale.x;
	matrix(2, 2) = myScale.y;
	matrix(3, 3) = myScale.z;

	if (aIsAnim)
	{
		matrix *= Matrix4x4<float>::CreateRotationAroundZ(DEG2RAD(myRotation.z));
		matrix *= Matrix4x4<float>::CreateRotationAroundY(DEG2RAD(myRotation.y));
		matrix *= Matrix4x4<float>::CreateRotationAroundX(DEG2RAD(myRotation.x));
	}
	else
	{
		matrix *= Matrix4x4<float>::CreateRotationAroundX(DEG2RAD(myRotation.x));
		matrix *= Matrix4x4<float>::CreateRotationAroundY(DEG2RAD(myRotation.y));
		matrix *= Matrix4x4<float>::CreateRotationAroundZ(DEG2RAD(myRotation.z));
	}

	matrix(4, 1) = myPosition.x;
	matrix(4, 2) = myPosition.y;
	matrix(4, 3) = myPosition.z;

	myMatrix = matrix;
}


Vector3f Transform::GetRotation() const
{
	return { static_cast<float>(fmod(myRotation.x, 360)), static_cast<float>(fmod(myRotation.y, 360)),
		static_cast<float>(fmod(myRotation.z, 360)) };
}

Vector3f Transform::GetPosition() const
{
	return myPosition;
}

Vector3f Transform::GetScale() const
{
	return myScale;
}

Matrix4x4f Transform::GetMatrix() const
{
	return myMatrix;
}

Matrix4x4f& Transform::GetMatrix()
{
	return myMatrix;
}

Vector3f Transform::GetForward() const
{
	Matrix4x4f rotation = myMatrix;
	rotation(4, 1) = 0;
	rotation(4, 2) = 0;
	rotation(4, 3) = 0;
	auto result = Vector4f(0, 0, 1.f, 1.f) * rotation;
	return { result.x, result.y, result.z };
}

Vector3f Transform::GetRight() const
{
	Matrix4x4f rotation = myMatrix;
	rotation(4, 1) = 0;
	rotation(4, 2) = 0;
	rotation(4, 3) = 0;
	auto result = Vector4f(1.f, 0, 0, 1.f) * rotation;
	return { result.x, result.y, result.z };
}

Vector3f Transform::GetUp() const
{
	Matrix4x4f rotation = myMatrix;
	rotation(4, 1) = 0;
	rotation(4, 2) = 0;
	rotation(4, 3) = 0;
	auto result = Vector4f(0, 1.f, 0, 1.f) * rotation;
	return { result.x, result.y, result.z };
}

Quatf Transform::GetQuaternion() const
{
	return Quatf(GetMatrix());
}