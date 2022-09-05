#include "GraphicsEngine.pch.h"
#include "RenderStateManager.h"
#include "DX11.h"

std::unordered_map<RenderStateManager::BlendState, ComPtr<ID3D11BlendState>> RenderStateManager::myBlendStates;
std::unordered_map<RenderStateManager::DepthStencilState, ComPtr<ID3D11DepthStencilState>> RenderStateManager::myDepthStencilStates;

bool RenderStateManager::Initialize()
{
	HRESULT result;

	// blend states
	D3D11_BLEND_DESC blendDesc = {};
	D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = true;
	renderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	renderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = renderTargetBlendDesc;
	result = DX11::Device->CreateBlendState(&blendDesc, myBlendStates[BlendState::AlphaBlend].GetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	renderTargetBlendDesc.BlendEnable = false;
	blendDesc.RenderTarget[0] = renderTargetBlendDesc;
	result = DX11::Device->CreateBlendState(&blendDesc, myBlendStates[BlendState::Opaque].GetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	D3D11_RENDER_TARGET_BLEND_DESC particleTargetDesc = {};
	D3D11_BLEND_DESC particleDesc = {};
	particleTargetDesc.BlendEnable = true;
	particleTargetDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	particleTargetDesc.DestBlend = D3D11_BLEND_ONE;
	particleTargetDesc.BlendOp = D3D11_BLEND_OP_ADD;
	particleTargetDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	particleTargetDesc.DestBlendAlpha = D3D11_BLEND_ONE;
	particleTargetDesc.BlendOpAlpha = D3D11_BLEND_OP_MAX;
	particleTargetDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	particleDesc.RenderTarget[0] = particleTargetDesc;

	result = DX11::Device->CreateBlendState(&particleDesc, myBlendStates[BlendState::Additive].GetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	D3D11_BLEND_DESC textDesc;
	ZeroMemory(&textDesc, sizeof(textDesc));
	textDesc.AlphaToCoverageEnable = false;
	textDesc.RenderTarget[0].BlendEnable = true;
	textDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	textDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	textDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	textDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
	textDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	textDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	textDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	result = DX11::Device->CreateBlendState(&textDesc, myBlendStates[BlendState::TextBlend].GetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = false;
	DX11::Device->CreateDepthStencilState(&depthStencilDesc, myDepthStencilStates[DepthStencilState::ReadWrite].GetAddressOf());

	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DX11::Device->CreateDepthStencilState(&depthStencilDesc, myDepthStencilStates[DepthStencilState::ReadOnly].GetAddressOf());

	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	DX11::Device->CreateDepthStencilState(&depthStencilDesc, myDepthStencilStates[DepthStencilState::None].GetAddressOf());
}

void RenderStateManager::SetBlendState(BlendState aState)
{
	DX11::Context->OMSetBlendState(myBlendStates[aState].Get(), nullptr, 0xFFFFFFFF);
}

void RenderStateManager::SetDepthStencilState(DepthStencilState aState)
{
	DX11::Context->OMSetDepthStencilState(myDepthStencilStates[aState].Get(), 0);
}
