#pragma once
#include "Transform.h"
#include "../External/entt/entity/entity.hpp"
#include <vector>

class SceneObject
{
protected:
	Transform myTransform;

public:
	SceneObject() = default;
	virtual ~SceneObject() = default;

	std::vector<std::shared_ptr<SceneObject>> myChildren;
	std::shared_ptr<SceneObject> myParent;
	entt::entity myEntity;

	void SetTransform(const Transform& aTransform);
	void SetRotation(float aPitch, float aYaw, float aRoll);
	void SetRotation(Vector3f aRot);
	void SetPosition(float x, float y, float z);
	void SetPosition(Vector3f aPos);
	void SetScale(float x, float y, float z);
	void SetScale(Vector3f aScale);

	Vector3f GetRotation() const;

	FORCEINLINE Transform const& GetTransform() const { return myTransform; }
	FORCEINLINE Transform& GetTransform() { return myTransform; }

	virtual std::wstring const& GetName() const { return L"nothing"; };
	virtual void SetName(const std::wstring& aName) {};
	virtual int const& GetId() const { return 0; };
	virtual void OnComponentAdded(entt::entity aEntity) {};
};