#include "GraphicsEngine.pch.h"
#include "RenderTarget.h"
#include "GraphicsEngine.h"

void RenderTarget::SetAsTarget()
{
	DX11::Context->OMSetRenderTargets(1, myRTV.GetAddressOf(), nullptr);
	DX11::Context->RSSetViewports(1, &myViewport);
}

void RenderTarget::RemoveTarget()
{
	ComPtr<ID3D11RenderTargetView> currentRTV;
	ComPtr<ID3D11DepthStencilView> currentDSV;
	DX11::Context->OMGetRenderTargets(1, currentRTV.GetAddressOf(), currentDSV.GetAddressOf());
	DX11::Context->OMSetRenderTargets(0, nullptr, currentDSV.Get());
}

void RenderTarget::SetAsResource(unsigned int aSlot)
{
	DX11::Context->PSSetShaderResources(aSlot, 1, mySRV.GetAddressOf());
}

void RenderTarget::RemoveResource(unsigned int aSlot)
{
	ID3D11ShaderResourceView* SRV = nullptr;
	DX11::Context->PSSetShaderResources(aSlot, 1, &SRV);
}

void RenderTarget::Clear()
{
	std::array<FLOAT, 4> gBufferClearColor = GraphicsEngine::GetClearColor();

	DX11::Context->ClearRenderTargetView(myRTV.Get(), &gBufferClearColor[0]);
	DX11::Context->ClearDepthStencilView(DX11::DepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	DX11::Context->ClearDepthStencilView(DX11::DepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 30);
}
