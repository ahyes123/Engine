#include "GraphicsEngine.pch.h"
#include "LightAssetHandler.h"

#include "UtilityFunctions.hpp"
#include "Texture/TextureAssetHandler.h"

std::shared_ptr<DirectionalLight> LightAssetHandler::CreateDirectionalLight(Vector3f aColor, float anIntensity,
																			Vector3f aRotation)
{
	myDirectionalLight = std::make_shared<DirectionalLight>();
	myDirectionalLight->Init(aColor, anIntensity);

	Transform transform;
	transform.SetRotation(aRotation);
	myDirectionalLight->myLightBufferData.Direction = transform.GetForward();

	return myDirectionalLight;
}

std::shared_ptr<EnvironmentLight> LightAssetHandler::CreateEnvironmentLight(const std::wstring& aTextureName)
{
	myEnvironmentLight = std::make_shared<EnvironmentLight>();

	bool elBool = TextureAssetHandler::LoadTexture(aTextureName);

	assert(elBool && L"Could not load environment light");

	myEnvironmentLight->myTexture = TextureAssetHandler::GetTexture(aTextureName);

	return myEnvironmentLight;
}
