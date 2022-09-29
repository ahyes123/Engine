#include "GraphicsEngine.pch.h"
#include "RenderTarget.h"
#include "DX11.h"
#include "GraphicsEngine.h"

void RenderTarget::SetAsTarget()
{
	DX11::Context->RSSetViewports(1, &myViewport);
	DX11::Context->OMSetRenderTargets(1, myRTV.GetAddressOf(), nullptr);
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
