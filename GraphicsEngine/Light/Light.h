#pragma once
#include "SceneObject.h"
#include <wrl.h>

using namespace Microsoft::WRL;

class Light : public SceneObject
{
	friend class LightAssetHandler;

public:
	struct LightBufferData
	{
		Vector3f Color;
		float Intensity;

		Vector3f Direction;
		float Range;

		Vector3f Position;
		float Attenuation;

		float SpotInnerRadius;
		float SpotOuterRadius;
		unsigned LightType;
		float Padding;
	};

protected:

public:
	LightBufferData myLightBufferData;

	virtual ~Light() override = default;
	virtual void Init(Vector3f aColor, float anIntensity);
	virtual void SetAsResource(ComPtr<ID3D11Buffer> aLightBuffer) = 0;

	FORCEINLINE Vector3f GetColor() const { return myLightBufferData.Color; }
	FORCEINLINE float GetIntensity() const { return myLightBufferData.Intensity; }
	FORCEINLINE LightBufferData GetLightBufferData() const { return myLightBufferData; }

};

