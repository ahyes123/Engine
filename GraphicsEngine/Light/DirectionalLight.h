#pragma once
#include "Light/Light.h"

class DirectionalLight : public Light
{
	public:
		void SetAsResource(ComPtr<ID3D11Buffer> aLightBuffer) override;
};

