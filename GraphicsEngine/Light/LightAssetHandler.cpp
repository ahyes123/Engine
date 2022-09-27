#include "GraphicsEngine.pch.h"
#include "LightAssetHandler.h"

#include "DX11.h"
#include "SceneHandler.h"
#include "UtilityFunctions.hpp"
#include "Texture/TextureAssetHandler.h"

void LightAssetHandler::Update()
{
	Transform transform = myDirectionalLight->GetTransform();
	transform.SetPosition(myDirectionalLight->GetTransform().GetPosition());
	myDirectionalLight->myLightBufferData.LightView[0] = Matrix4x4f::GetFastInverse(transform.GetMatrix());
	myDirectionalLight->myLightBufferData.Direction = transform.GetForward();

	for (auto& light : myLights)
	{
		transform = light->GetTransform();
		transform.SetPosition(light->myLightBufferData.Position);
		switch (light->myLightBufferData.LightType)
		{
		case 2:
		{
			light->myLightBufferData.LightView[0] = Matrix4x4f::GetFastInverse(transform.GetMatrix());
			light->myLightBufferData.Direction = transform.GetForward();
			break;
		}
		case 1:
		{
			transform.SetRotation({ 0, 0, 0 });
			light->myLightBufferData.LightView[0] = Matrix4x4f::GetFastInverse(transform.GetMatrix());
			transform.SetRotation({ 0, 180, 0 });
			light->myLightBufferData.LightView[1] = Matrix4x4f::GetFastInverse(transform.GetMatrix());
			transform.SetRotation({ 0, 90, 0 });
			light->myLightBufferData.LightView[2] = Matrix4x4f::GetFastInverse(transform.GetMatrix());
			transform.SetRotation({ 0, 270, 0 });
			light->myLightBufferData.LightView[3] = Matrix4x4f::GetFastInverse(transform.GetMatrix());
			transform.SetRotation({ 270, 0, 0 });
			light->myLightBufferData.LightView[4] = Matrix4x4f::GetFastInverse(transform.GetMatrix());
			transform.SetRotation({ 90, 0, 0 });
			light->myLightBufferData.LightView[5] = Matrix4x4f::GetFastInverse(transform.GetMatrix());
			break;
		}
		}
	}
}

std::shared_ptr<DirectionalLight> LightAssetHandler::CreateDirectionalLight(Vector3f aColor, float anIntensity,
	Vector3f aRotation, Vector3f aPosition)
{
	myDirectionalLight = std::make_shared<DirectionalLight>();
	myDirectionalLight->Init(aColor, anIntensity);

	myDirectionalLight->GetTransform().SetRotation(aRotation);
	myDirectionalLight->myLightBufferData.Direction = myDirectionalLight->GetTransform().GetForward();

	myDirectionalLight->SetPosition(aPosition);

	constexpr float nearPlane = 1.f;
	constexpr  float farPlane = 25000.0f;

	const POINT resolution = { 2048, 2048 };

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
	myDirectionalLight->myLightBufferData.LightView[0] = Matrix4x4f::GetFastInverse(myDirectionalLight->GetTransform().GetMatrix());

	myDirectionalLight->myShadowMap = TextureAssetHandler::CreateDepthStencil(L"shadow", resolution.x, resolution.y);

	myDirectionalLight->myLightBufferData.CastShadows = false;

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

	result->GetTransform().SetPosition(aPosition);
	result->myLightBufferData.Position = aPosition;

	constexpr float nearPlane = 0.1f;
	constexpr float farPlane = 25000.f;
	const POINT resolution = { 2048, 2048 };

	result->myLightBufferData.NearPlane = nearPlane;
	result->myLightBufferData.FarPlane = farPlane;

#define DEG2RAD(a) (a * PI / 180.0f)

	Matrix4x4f lightProjection;

	const float hFoV = DEG2RAD(90.f);
	const float vFoV = 2 * std::atan(std::tan(hFoV * 0.5f) * (static_cast<float>(resolution.x) / static_cast<float>(resolution.y)));

	const float myXScale = 1 / std::tanf(hFoV * 0.5f);
	const float myYScale = 1 / std::tanf(vFoV * 0.5f);
	const float Q = farPlane / (farPlane - nearPlane);

	lightProjection(1, 1) = myXScale;
	lightProjection(2, 2) = myYScale;
	lightProjection(3, 3) = Q;
	lightProjection(4, 3) = -Q * nearPlane;
	lightProjection(3, 4) = 1.f / Q;
	lightProjection(4, 4) = 0;

	result->myLightBufferData.LightProjection = lightProjection;

	Transform transform = result->GetTransform();

	transform.SetRotation({ 0, 0, 0 });
	result->myLightBufferData.LightView[0] = Matrix4x4f::GetFastInverse(transform.GetMatrix());
	transform.SetRotation({ 0, 180, 0 });
	result->myLightBufferData.LightView[1] = Matrix4x4f::GetFastInverse(transform.GetMatrix());
	transform.SetRotation({ 0, 90, 0 });
	result->myLightBufferData.LightView[2] = Matrix4x4f::GetFastInverse(transform.GetMatrix());
	transform.SetRotation({ 0, 270, 0 });
	result->myLightBufferData.LightView[3] = Matrix4x4f::GetFastInverse(transform.GetMatrix());
	transform.SetRotation({ 270, 0, 0 });
	result->myLightBufferData.LightView[4] = Matrix4x4f::GetFastInverse(transform.GetMatrix());
	transform.SetRotation({ 90, 0, 0 });
	result->myLightBufferData.LightView[5] = Matrix4x4f::GetFastInverse(transform.GetMatrix());

	result->myLightBufferData.CastShadows = true;

	result->myShadowMap = TextureAssetHandler::CreateDepthStencil(L"point", resolution.x, resolution.y);
	for (int i = 0; i < 5; i++)
	{
		result->myExtraShadowMaps[i] = TextureAssetHandler::CreateDepthStencil(L"point", resolution.x, resolution.y);
	}
	result->SetRange(aRange);
	result->myLightBufferData.Attenuation = aAttenuation;
	result->myLightBufferData.LightType = 1;

	return result;
}

std::shared_ptr<SpotLight> LightAssetHandler::CreateSpotLight(Vector3f aColor, float anIntensity, float aRange, float aAttenuation,
	float aInnerCone, float aOuterCone, Vector3f aRotation, Vector3f aPosition)
{
	myLights.push_back(std::make_shared<SpotLight>());
	std::shared_ptr<SpotLight> result = std::dynamic_pointer_cast<SpotLight>(myLights.back());
	result->Init(aColor, anIntensity);

	result->GetTransform().SetPosition(aPosition);
	result->myLightBufferData.Position = aPosition;
	result->GetTransform().SetRotation(aRotation);
	result->SetDirection(result->GetTransform().GetForward());

	constexpr float nearPlane = 1.f;
	constexpr float farPlane = 25000.f;
	const POINT resolution = { 2048, 2048 };

	result->myLightBufferData.NearPlane = nearPlane;
	result->myLightBufferData.FarPlane = farPlane;

#define DEG2RAD(a) (a * PI / 180.0f)

	Matrix4x4f lightProjection;

	const float hFoV = DEG2RAD(90.f);
	const float vFoV = 2 * std::atan(std::tan(hFoV * 0.5f) * (static_cast<float>(resolution.y) / static_cast<float>(resolution.x)));

	const float myXScale = 1 / std::tanf(hFoV * 0.5f);
	const float myYScale = 1 / std::tanf(vFoV * 0.5f);
	const float Q = farPlane / (farPlane - nearPlane);

	lightProjection(1, 1) = myXScale;
	lightProjection(2, 2) = myYScale;
	lightProjection(3, 3) = Q;
	lightProjection(3, 4) = 1 / Q;
	lightProjection(4, 3) = -Q * nearPlane;
	lightProjection(4, 4) = 0;

	result->myLightBufferData.LightProjection = lightProjection;
	result->myLightBufferData.LightView[0] = Matrix4x4f::GetFastInverse(result->GetTransform().GetMatrix());
	result->myLightBufferData.CastShadows = false;

	result->myShadowMap = TextureAssetHandler::CreateDepthStencil(L"SpotShadow", resolution.x, resolution.y);

	result->SetRange(aRange);
	result->SetInnerCone(aInnerCone);
	result->SetOuterCone(aOuterCone);
	result->myLightBufferData.Attenuation = aAttenuation;
	result->myLightBufferData.LightType = 2;

	return result;
}