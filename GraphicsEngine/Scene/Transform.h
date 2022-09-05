#pragma once
#include "Quaternion.hpp"
#include "Math/Matrix4x4.hpp"
#include "Math/Vector3.hpp"

class Transform
{
public:
	Transform() = default;
	Transform(Matrix4x4f aMatrix);
	void SetPosition(const Vector3f& position);
	void SetRotation(const Vector3f& rotation);
	void SetScale(const Vector3f& scale);
	void ComposeMatrix(bool aIsAnim = false);
	Vector3f GetRotation() const;
	Vector3f GetRawRotation() const;
	Vector3f GetPosition() const;
	Vector3f GetScale() const;
	Matrix4x4f GetMatrix() const;
	Matrix4x4f& GetMatrix();
	Vector3f GetForward() const;
	Vector3f GetRight() const;
	Vector3f GetUp() const;
	Vector3f& GetPositionMutable();
	Vector3f& GetRotationMutable();
	Vector3f& GetScaleMutable();

	Quatf GetQuaternion() const;

private:
	Matrix4x4f myMatrix;
	Vector3f myPosition;
	Vector3f myScale = { 1,1,1 };
	Vector3f myRotation;
};

