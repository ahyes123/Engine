#pragma once

#include "Light/Light.h"
#include "Texture/Texture.h"

class EnvironmentLight : public Light
{
	friend class LightAssetHandler;
	std::shared_ptr<Texture> myTexture;

	public:
		void SetAsResource(ComPtr<ID3D11Buffer> aLightBuffer) override;
};

