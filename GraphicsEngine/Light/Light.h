#pragma once
#include "SceneObject.h"
#include <wrl.h>
#include "Texture/DepthStencil.h"

using namespace Microsoft::WRL;

class Light : public SceneObject
{
	friend class LightAssetHandler;

public:
	struct LightBufferData
	{
		Matrix4x4f LightView;
		Matrix4x4f LightProjection;

		Vector3f Color;
		float Intensity;

		Vector3f Direction;
		float Range;

		Vector3f Position;
		float Attenuation;

		float SpotInnerRadius;
		float SpotOuterRadius;
		unsigned LightType;
		bool CastShadows;

		float NearPlane;
		float FarPlane;
		Vector2f Padding;
	};

protected:
	std::unique_ptr<DepthStencil> myShadowMap;

public:
	LightBufferData myLightBufferData;

	virtual ~Light() override = default;
	virtual void Init(Vector3f aColor, float anIntensity);
	virtual void SetAsResource(ComPtr<ID3D11Buffer> aLightBuffer) = 0;

	void ClearShadowMap();
	void SetShadowMapAsDepth();

	FORCEINLINE Vector3f GetColor() const { return myLightBufferData.Color; }
	FORCEINLINE float GetIntensity() const { return myLightBufferData.Intensity; }
	FORCEINLINE LightBufferData GetLightBufferData() const { return myLightBufferData; }

};

