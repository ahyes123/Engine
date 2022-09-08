#pragma once
#include "Light.h"

class PointLight : public Light
{
	friend class LightAssetHandler;

public:
	virtual ~PointLight() = default;

	void SetRange(float aRange) { myLightBufferData.Range = aRange; }
	void SetAsResource(ComPtr<ID3D11Buffer> aLightBuffer) override { aLightBuffer; }

	FORCEINLINE float GetRange() const { return myLightBufferData.Range; }
};

