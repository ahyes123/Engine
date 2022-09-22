#include "GraphicsEngine.pch.h"
#include "LightAssetHandler.h"

#include "DX11.h"
#include "UtilityFunctions.hpp"
#include "Texture/TextureAssetHandler.h"

std::shared_ptr<DirectionalLight> LightAssetHandler::CreateDirectionalLight(Vector3f aColor, float anIntensity,
																			Vector3f aRotation, Vector3f aPosition)
{
	myDirectionalLight = std::make_shared<DirectionalLight>();
	myDirectionalLight->Init(aColor, anIntensity);

	myDirectionalLight->GetTransform().SetRotation(aRotation);
	myDirectionalLight->myLightBufferData.Direction = myDirectionalLight->GetTransform().GetForward();

	myDirectionalLight->SetPosition(aPosition);

	constexpr float nearPlane = 1.0f;
	constexpr  float farPlane = 25000.0f;
	const POINT resolution = { DX11::ClientRect.right - DX11::ClientRect.left, DX11::ClientRect.bottom - DX11::ClientRect.top };

	myDirectionalLight->myLightBufferData.NearPlane = nearPlane;
	myDirectionalLight->myLightBufferData.FarPlane = farPlane;

	Matrix4x4f lightProjection;

	lightProjection(1, 1) = 2.0f / static_cast<float>(resolution.x);
	lightProjection(2, 2) = 2.0f / static_cast<float>(resolution.y);
	lightProjection(3, 3) = 1.0f / (farPlane - nearPlane);
	lightProjection(4, 3) = nearPlane / (nearPlane - farPlane);
	lightProjection(4, 4) = 1.0f;

	myDirectionalLight->myLightBufferData.LightProjection = lightProjection;
	myDirectionalLight->myLightBufferData.Position = myDirectionalLight->GetTransform().GetPosition();
	myDirectionalLight->myLightBufferData.LightView = Matrix4x4f::GetFastInverse(myDirectionalLight->GetTransform().GetMatrix());

	myDirectionalLight->myShadowMap = TextureAssetHandler::CreateDepthStencil(L"shadow", resolution.x, resolution.y);

	myDirectionalLight->myLightBufferData.CastShadows = true;

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

std::shared_ptr<PointLight> LightAssetHandler::CreatePointLight(Vector3f aColor, float anIntensity, float aRange, float aAttenuation,
	Vector3f aPosition)
{
	myLights.push_back(std::make_shared<PointLight>());
	std::shared_ptr<PointLight> result = std::dynamic_pointer_cast<PointLight>(myLights.back());

	result->Init(aColor, anIntensity);
	result->SetRange(aRange);
	result->SetPosition(aPosition);
	result->myLightBufferData.Position = aPosition;
	result->myLightBufferData.Attenuation = aAttenuation;
	result->myLightBufferData.LightType = 1;

	return result;
}

std::shared_ptr<SpotLight> LightAssetHandler::CreateSpotLight(Vector3f aColor, float anIntensity, float aRange, float aAttenuation,
	float aInnerCone, float aOuterCone, Vector3f aDirection, Vector3f aPosition)
{
	myLights.push_back(std::make_shared<SpotLight>());
	std::shared_ptr<SpotLight> result = std::dynamic_pointer_cast<SpotLight>(myLights.back());

	result->Init(aColor, anIntensity);
	result->SetRange(aRange);
	result->SetInnerCone(aInnerCone);
	result->SetOuterCone(aOuterCone);
	result->SetPosition(aPosition);
	result->SetDirection(aDirection);
	result->myLightBufferData.Position = aPosition;
	result->myLightBufferData.Attenuation = aAttenuation;
	result->myLightBufferData.LightType = 2;

	return result;
}