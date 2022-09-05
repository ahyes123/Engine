#include "GraphicsEngine.pch.h"
#include "TextureAssetHandler.h"
#include "DDSTextureLoader11.h"
#include "DX11.h"
#include "Texture.h"

std::shared_ptr<Texture> TextureAssetHandler::GetTexture(const std::wstring& aName)
{
	return myRegistry[aName];
}

bool TextureAssetHandler::LoadTexture(const std::wstring& aFileName)
{
	if (const auto It = myRegistry.find(aFileName); It == myRegistry.end())
	{
		Texture result;
		result.myName = aFileName;
		const HRESULT createResult = DirectX::CreateDDSTextureFromFile(DX11::Device.Get(), aFileName.c_str(), result.myTexture.GetAddressOf(), result.mySRV.GetAddressOf());

		if (SUCCEEDED(createResult))
		{
			myRegistry.insert({ aFileName, std::make_shared<Texture>(result) });
			return true;
		}

	}
	else if (myRegistry[aFileName].get() != nullptr)
	{
		return true;
	}
	return false;
}

std::unique_ptr<GBuffer> TextureAssetHandler::CreateGBuffer(RECT aWindowSize)
{
	std::unique_ptr<GBuffer> gbuffer = std::make_unique<GBuffer>();
	
	for (uint8_t i = 0; i < GBuffer::GBufferTexture::GB_COUNT; ++i)
	{
		ComPtr<ID3D11Texture2D> texture;
		D3D11_TEXTURE2D_DESC textureDesc = { 0 };
		textureDesc.Width = aWindowSize.right - aWindowSize.left;
		textureDesc.Height = aWindowSize.bottom - aWindowSize.top;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.MipLevels = 1;

		HRESULT result = DX11::Device->CreateTexture2D(&textureDesc, nullptr, texture.GetAddressOf());
		if (FAILED(result))
		{
			return nullptr;
		}

		result = DX11::Device->CreateRenderTargetView(texture.Get(), nullptr, &gbuffer->myRTVs[i]);
		if (FAILED(result))
		{
			return nullptr;
		}
		result = DX11::Device->CreateShaderResourceView(texture.Get(), nullptr, &gbuffer->mySRVs[i]);
		if (FAILED(result))
		{
			return nullptr;
		}
	}

	return gbuffer;
}
