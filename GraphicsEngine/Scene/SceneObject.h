#pragma once
#include "Transform.h"
#include "../External/entt/entity/entity.hpp"

class SceneObject
{
protected:
	Transform myTransform;
	int myId = 0;
	std::wstring myName = L"nothing";

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

	virtual std::wstring const& GetName() const { return myName; };
	virtual void SetName(const std::wstring& aName) { aName; };
	int const& GetId() const { return myId; }
	void SetId(int aId) { myId = aId; }
	virtual void OnComponentAdded(entt::entity aEntity) { aEntity; };
};
