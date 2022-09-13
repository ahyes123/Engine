#pragma once
#include <string>
#include <wrl.h>
#include <d3d11.h>

using namespace Microsoft::WRL;

struct DepthStencil
{
	ComPtr<ID3D11ShaderResourceView> mySRV;
	ComPtr<ID3D11DepthStencilView> myDRV;
	ComPtr<ID3D11Texture2D> myTexture;
	std::wstring myName;
	D3D11_VIEWPORT myViewPort;
};

