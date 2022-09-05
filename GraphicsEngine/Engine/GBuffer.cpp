#include "GraphicsEngine.pch.h"
#include "GBuffer.h"

#include "DX11.h"
#include "GraphicsEngine.h"

ComPtr<ID3D11ShaderResourceView> GBuffer::GBufferVPSRV;
ComPtr<ID3D11RenderTargetView> GBuffer::GBufferVPRTV;

bool GBuffer::Init()
{
	RECT rect = DX11::ClientRect;
	ComPtr<ID3D11Texture2D> texture;
	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	textureDesc.Width = rect.right - rect.left;
	textureDesc.Height = rect.bottom - rect.top;
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
		return false;
	}

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	result = DX11::Device->CreateRenderTargetView(texture.Get(), &renderTargetViewDesc, GBufferVPRTV.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	result = DX11::Device->CreateShaderResourceView(texture.Get(), &shaderResourceViewDesc, GBufferVPSRV.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void GBuffer::SetAsTarget() const
{
	ID3D11RenderTargetView* myRTVList[GBufferTexture::GB_COUNT];
	for (unsigned t = 0; t < myRTVs.size(); t++)
	{
		myRTVList[t] = myRTVs[t].Get();
	}
	DX11::Context->OMSetRenderTargets(GBufferTexture::GB_COUNT, &myRTVList[0], DX11::DepthBuffer.Get());
}

void GBuffer::ClearTarget() const
{
	ID3D11RenderTargetView* myRTVList[GBufferTexture::GB_COUNT];
	for (unsigned t = 0; t < myRTVs.size(); t++)
	{
		myRTVList[t] = nullptr;
	}
	DX11::Context->OMSetRenderTargets(GBufferTexture::GB_COUNT, &myRTVList[0], DX11::DepthBuffer.Get());
}

void GBuffer::SetAsResource(unsigned aStartSlot) const
{
	ID3D11ShaderResourceView* mySRVList[GBufferTexture::GB_COUNT];
	for (unsigned t = 0; t < mySRVs.size(); t++)
	{
		mySRVList[t] = mySRVs[t].Get();
	}

	DX11::Context->PSSetShaderResources(aStartSlot, GBufferTexture::GB_COUNT, &mySRVList[0]);
}

void GBuffer::ClearResource(unsigned aStartSlot) const
{
	DX11::Context->ClearDepthStencilView(DX11::DepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, aStartSlot);
}

void GBuffer::Clear() const
{
	std::array<FLOAT, 4> clearColor = {0, 0, 0, 0};
	ID3D11RenderTargetView* myRTVList[GBufferTexture::GB_COUNT];
	for (unsigned t = 0; t < myRTVs.size(); t++)
	{
		myRTVList[t] = myRTVs[t].Get();
	}
	DX11::Context->ClearRenderTargetView(myRTVList[0], &clearColor[0]);
	DX11::Context->ClearRenderTargetView(GBufferVPRTV.Get(), &clearColor[0]);

	ClearTarget();
	ClearResource(0);
}

ComPtr<ID3D11ShaderResourceView> GBuffer::GetVPSRV()
{
	return GBufferVPSRV;
}

ComPtr<ID3D11RenderTargetView> GBuffer::GetVPRTV()
{
	return GBufferVPRTV;
}