#include "GraphicsEngine.pch.h"
#include "MeshComponent.h"
#include "ModelAssetHandler.h"

MeshComponent::MeshComponent()
	: Component("MeshComponent")
{
	EditorVariable("Path", ParameterType::String, &myPath);
	EditorButton("Reload", [&]() {ReloadMesh(); });
}

void MeshComponent::Init()
{
}

void MeshComponent::Update()
{
}

void MeshComponent::SetModel(std::shared_ptr<Model> aModel)
{
	myModel = aModel;
	myPath = myModel->GetPath();
}

void MeshComponent::ReloadMesh()
{
	SetModel(ModelAssetHandler::LoadModel(myPath));
}