#include "GraphicsEngine.pch.h"
#include "Light.h"

#include "DX11.h"
#include "GBuffer.h"

void Light::Init(Vector3f aColor, float anIntensity)
{
	myLightBufferData.Color = aColor;
	myLightBufferData.Intensity = anIntensity;
}

void Light::ClearShadowMap(unsigned aIndex)
{
	if (aIndex == 0)
		DX11::Context->ClearDepthStencilView(myShadowMap->myDRV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	else if (myExtraShadowMaps[aIndex - 1])
		DX11::Context->ClearDepthStencilView(myExtraShadowMaps[aIndex - 1]->myDRV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Light::SetShadowMapAsDepth(unsigned aIndex)
{
	ID3D11RenderTargetView* impostorBackBuffer = nullptr;

	if (aIndex == 0)
		DX11::Context->OMSetRenderTargets(1, &impostorBackBuffer, myShadowMap->myDRV.Get());
	else if (myExtraShadowMaps[aIndex - 1])
		DX11::Context->OMSetRenderTargets(1, &impostorBackBuffer, myExtraShadowMaps[aIndex - 1]->myDRV.Get());
}

void Light::SetShadowMapAsResource(int aSlot)
{
	DX11::Context->PSSetShaderResources(aSlot, 1, myShadowMap->mySRV.GetAddressOf());

	for (int i = 0; i < 5; ++i)
		if (myExtraShadowMaps[i])
			DX11::Context->PSSetShaderResources(aSlot + i + 1, 1, myExtraShadowMaps[i]->mySRV.GetAddressOf());
		else
			break;
}
