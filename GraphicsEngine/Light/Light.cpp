#include "GraphicsEngine.pch.h"
#include "Light.h"

#include "DX11.h"
#include "GBuffer.h"

void Light::Init(Vector3f aColor, float anIntensity)
{
	myLightBufferData.Color = aColor;
	myLightBufferData.Intensity = anIntensity;
}

void Light::ClearShadowMap()
{
	DX11::Context->ClearDepthStencilView(myShadowMap->myDRV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Light::SetShadowMapAsDepth()
{
	DX11::Context->OMSetRenderTargets(1, GBuffer::GetVPRTV().GetAddressOf(), myShadowMap->myDRV.Get());
}

void Light::SetShadowMapAsResource(int aSlot)
{
	DX11::Context->PSSetShaderResources(aSlot, 1, myShadowMap->mySRV.GetAddressOf());
}
