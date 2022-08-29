#include "GraphicsEngine.pch.h"
#include "Material.h"

#include "DX11.h"
#include "Texture/Texture.h"

Material::Material()
{
}

void Material::Init(const std::wstring& aName, const Vector3f& anAlbedo)
{
	myName = aName;
	myMaterialData.myAlbedo = anAlbedo;
}

void Material::SetAlbedoTexture(std::shared_ptr<Texture> aTexture)
{
	myTextures[0] = aTexture;
}

void Material::SetNormalTexture(std::shared_ptr<Texture> aNormalTexture)
{
	myTextures[1] = aNormalTexture;
}

void Material::SetMaterialTexture(std::shared_ptr<Texture> aMaterialTexture)
{
	myTextures[2] = aMaterialTexture;
}

void Material::SetAsResource(ComPtr<ID3D11Resource> aMaterialBuffer) const
{
	D3D11_MAPPED_SUBRESOURCE bufferData;
	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));

	const HRESULT result = DX11::Context->Map(aMaterialBuffer.Get(), 0,	D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

	if (FAILED((result)))
	{
		//boom
	}

	memcpy_s(bufferData.pData, sizeof(MaterialData), &myMaterialData, sizeof(MaterialData));

	DX11::Context->Unmap(aMaterialBuffer.Get(), 0);

	if (myTextures[MaterialTextureChannel::Albedo])
	{
		myTextures[MaterialTextureChannel::Albedo]->SetAsResource(0);
	}
	if (myTextures[MaterialTextureChannel::Normal])
	{
		myTextures[MaterialTextureChannel::Normal]->SetAsResource(1);
	}
	if (myTextures[MaterialTextureChannel::Material])
	{
		myTextures[MaterialTextureChannel::Material]->SetAsResource(2);
	}
}
