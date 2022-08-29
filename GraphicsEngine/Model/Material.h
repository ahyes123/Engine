#pragma once
#include <memory>
#include <array>
#include <string>
#include <wrl/client.h>
#include "Math/Vector3.hpp"

using namespace Microsoft::WRL;

class Texture;

namespace MaterialTextureChannel
{
	enum
	{
		Albedo,
		Normal,
		Material,
		COUNT
	};
}

class Material
{
	std::wstring myName;

public:
	struct MaterialData
	{
		Vector3f myAlbedo;
		float padding;
	};

private:
	MaterialData myMaterialData;
	std::array<std::shared_ptr<Texture>, MaterialTextureChannel::COUNT> myTextures;

public:
	Material();
	virtual ~Material() = default;
	void Init(const std::wstring& aName, const Vector3f& anAlbedo);

	FORCEINLINE const std::wstring& GetName() const { return myName; }
	FORCEINLINE const Vector3f& GetAlbedo() const { return myMaterialData.myAlbedo; }

	void SetAlbedoTexture(std::shared_ptr<Texture> aTexture);
	void SetNormalTexture(std::shared_ptr<Texture> aNormalTexture);
	void SetMaterialTexture(std::shared_ptr<Texture> aMaterialTexture);
	void SetAsResource(ComPtr<ID3D11Resource> aMaterialBuffer) const;
};

