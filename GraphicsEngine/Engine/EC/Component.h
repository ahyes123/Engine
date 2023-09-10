#pragma once
#include <string>
#include <functional>

enum class ParameterType
{
	Float,
	Vec3,
	Int,
	Bool,
	String,
	Button
};

struct Variable
{
	std::string Name;
	ParameterType Type;
	void* Value;
	float Increment;
	float Min;
	float Max;

	std::function<void()> ButtonFunc;
};

class Entity;
class Component
{
public:
	Component(const std::string& aName);
	Component(Component& aComp);
	virtual ~Component() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;

	Entity* GetEntity() { return myEntity; }
	void SetEntity(Entity* aEntity) { myEntity = aEntity; }

	std::string GetName() { return myName; }

	std::vector<Variable>& GetVariables() { return myVariables; }

protected:
	Entity* myEntity;
	std::string myName;

	void EditorVariable(std::string aName, ParameterType aType, void* aVal, float aIncrement = 5, float aMin = -FLT_MAX, float aMax = FLT_MAX);
	void EditorButton(std::string aName, std::function<void()> aButtonFunc);
private:
	std::vector<Variable> myVariables;
};