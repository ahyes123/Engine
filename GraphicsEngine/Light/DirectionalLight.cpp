#include "GraphicsEngine.pch.h"
#include "DirectionalLight.h"

void DirectionalLight::SetAsResource(ComPtr<ID3D11Buffer> aLightBuffer)
{
	D3D11_MAPPED_SUBRESOURCE bufferData;
	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));

	const HRESULT result = DX11::Context->Map(aLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if(FAILED(result))
	{
		
	}

	memcpy_s(bufferData.pData, sizeof(LightBufferData), &myLightBufferData, sizeof(LightBufferData));

	DX11::Context->Unmap(aLightBuffer.Get(), 0);

	DX11::Context->PSSetConstantBuffers(3, 1, aLightBuffer.GetAddressOf());
}
