#pragma once
#include <wrl/client.h>

#include "Camera.h"
#include "Matrix4x4.hpp"
#include "ModelInstance.h"
#include "Light/DirectionalLight.h"
#include "Light/EnvironmentLight.h"
#include "GBuffer.h"

using namespace Microsoft::WRL;

struct ID3D11PixelShader;

class DeferredRenderer
{
	struct FrameBufferData
	{
		Matrix4x4f View;
		Matrix4x4f Projection;
		Vector3f CamTranslation;
		unsigned int RenderMode;
		float NearPlane;
		float FarPlane;
		float DeltaTime;
		float TotalTime;
		CommonUtilities::Vector2<unsigned> Resolution;
		Vector2f padding;
		Vector4f FrustrumCorners[4];
	} myFrameBufferData;

	struct ObjectBufferData
	{
		Matrix4x4f World;
		bool HasBones = false;
		Vector3f padding;
		Matrix4x4f BoneData[128];
	} myObjectBufferData;

	struct SceneLightBuffer
	{
		Light::LightBufferData DirectionalLight;
		Light::LightBufferData Lights[8];

		unsigned int NumLights;
		Vector3f Padding;
	} mySceneLightBufferData;

	ComPtr<ID3D11Buffer> myFrameBuffer;
	ComPtr<ID3D11Buffer> myObjectBuffer;
	ComPtr<ID3D11Buffer> myMaterialBuffer;
	ComPtr<ID3D11Buffer> myLightBuffer;

	ComPtr<ID3D11PixelShader> myGBufferPixelShader;
	ComPtr<ID3D11VertexShader> myFullscreenShader;
	ComPtr<ID3D11PixelShader> myEnvironmentShader;
	ComPtr<ID3D11InputLayout> myGBufferLayout;

public:
	std::unique_ptr<GBuffer> myGBuffer;

	bool Initialize();
	void GenerateGBuffer(const std::shared_ptr<Camera>& aCamera, const std::vector<std::shared_ptr<ModelInstance>>& aModelList, float aDeltaTime, float aTotalTime);
	void Render(const std::shared_ptr<Camera>& aCamera, const std::shared_ptr<DirectionalLight>& aDirectionalLight, 
		const std::vector<std::shared_ptr<Light>>& aLightList, const std::shared_ptr<EnvironmentLight>& aEnvironmentLight, 
		float aDeltaTime, float aTotalTime);

	void ClearGBuffer();
};

