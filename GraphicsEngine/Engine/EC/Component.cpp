#include "GraphicsEngine.pch.h"
#include "Component.h"

Component::Component(const std::string& aName)
	: myName(aName)
{
	myEntity = nullptr;
}

Component::Component(Component& aComp)
{
	myName = aComp.myName;
	myEntity = aComp.myEntity;
}

void Component::EditorVariable(std::string aName, ParameterType aType, void* aVal, float aIncrement, float aMin, float aMax)
{
	Variable var;
	var.Name = aName;
	var.Type = aType;
	var.Value = aVal;
	var.Increment = aIncrement;
	var.Min = aMin;
	var.Max = aMax;

	myVariables.push_back(var);
}

void Component::EditorButton(std::string aName, std::function<void()> aButtonFunc)
{
	Variable var;
	var.Name = aName;
	var.Type = ParameterType::Button;
	var.ButtonFunc = aButtonFunc;

	myVariables.push_back(var);
}