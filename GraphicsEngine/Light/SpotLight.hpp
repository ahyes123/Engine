#pragma once
#include "Light.h"

class SpotLight : public Light
{
	friend class LightAssetHandler;
public:
	virtual ~SpotLight() = default;

	void SetRange(float aRange) { myLightBufferData.Range = aRange; }
	void SetInnerCone(float aRadius) { myLightBufferData.SpotInnerRadius = aRadius; }
	void SetOuterCone(float aRadius) { myLightBufferData.SpotOuterRadius = aRadius; }
	void SetDirection(Vector3f aDirection) { myLightBufferData.Direction = aDirection; }
	void SetAsResource(ComPtr<ID3D11Buffer> aLightBuffer) override { aLightBuffer; }

	FORCEINLINE float GetRange() const { return myLightBufferData.Range; }
	FORCEINLINE float GetInnerCone() const { return myLightBufferData.SpotInnerRadius; }
	FORCEINLINE float GetOuterCone() const { return myLightBufferData.SpotOuterRadius; }
};

