#include "GraphicsEngine.pch.h"
#include "TextureAssetHandler.h"
#include "../External/Tool/DDSTextureLoader11.h"
#include "DX11.h"
#include "Texture.h"

std::shared_ptr<Texture> TextureAssetHandler::GetTexture(const std::string& aName)
{
	return myRegistry[aName];
}

bool TextureAssetHandler::LoadTexture(const std::string& aFileName)
{
	if (const auto It = myRegistry.find(aFileName); It == myRegistry.end())
	{
		Texture result;
		result.myName = aFileName;
		const HRESULT createResult = DirectX::CreateDDSTextureFromFile(DX11::Device.Get(), 
			std::wstring(aFileName.begin(), aFileName.end()).c_str(), result.myTexture.GetAddressOf(), result.mySRV.GetAddressOf());

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

std::shared_ptr<DepthStencil> TextureAssetHandler::CreateDepthStencil(const std::wstring& aName, size_t aWidth,
																	  size_t aHeight)
{
	HRESULT result = S_FALSE;

	std::shared_ptr<DepthStencil> output = std::make_shared<DepthStencil>();
	output->myName = aName;

	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = static_cast<unsigned int>(aWidth);
	desc.Height = static_cast<unsigned int>(aHeight);
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	result = DX11::Device->CreateTexture2D(&desc, nullptr, reinterpret_cast<ID3D11Texture2D**>(output->myTexture.GetAddressOf()));
	assert(SUCCEEDED(result));

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc = {};
	resourceDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
	resourceDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceDesc.Texture2D.MipLevels = desc.MipLevels;

	result = DX11::Device->CreateShaderResourceView(output->myTexture.Get(), &resourceDesc, output->mySRV.GetAddressOf());
	assert(SUCCEEDED(result));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = { };
	depthDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	depthDesc.ViewDimension = D3D11_DSV_DIMENSION::D3D11_DSV_DIMENSION_TEXTURE2D;

	result = DX11::Device->CreateDepthStencilView(output->myTexture.Get(), &depthDesc, output->myDRV.GetAddressOf());
	assert(SUCCEEDED(result));

	output->myViewPort = D3D11_VIEWPORT({ 0.0f, 0.0f, static_cast<float>(aWidth), static_cast<float>(aHeight), 0.0f, 1.0f });

	return output;
}

std::unique_ptr<RenderTarget> TextureAssetHandler::CreateRenderTarget(UINT aWidth, UINT aHeight, DXGI_FORMAT aFormat)
{
	std::unique_ptr<RenderTarget> renderTarget = std::make_unique<RenderTarget>();

	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	textureDesc.Width = aWidth;
	textureDesc.Height = aHeight;
	textureDesc.ArraySize = 1;
	textureDesc.Format = aFormat;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.MipLevels = 1;

	HRESULT result = DX11::Device->CreateTexture2D(&textureDesc, nullptr, renderTarget->myTexture.GetAddressOf());

	if (FAILED(result))
	{
		return nullptr;
	}

	result = DX11::Device->CreateRenderTargetView(renderTarget->myTexture.Get(), nullptr, renderTarget->myRTV.GetAddressOf());

	if (FAILED(result))
	{
		return nullptr;
	}

	result = DX11::Device->CreateShaderResourceView(renderTarget->myTexture.Get(), nullptr, renderTarget->mySRV.GetAddressOf());

	if (FAILED(result))
	{
		return nullptr;
	}

	renderTarget->myViewport = D3D11_VIEWPORT({ 0.0f,0.0f, static_cast<float>(aWidth), static_cast<float>(aHeight), 0.0f, 1.0f });

	return renderTarget;
}
