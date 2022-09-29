#pragma once
#include <wrl.h>
#include <string>

using namespace Microsoft::WRL;

class RenderTarget
{
public:
	void SetAsTarget();
	void SetAsResource(unsigned int aSlot);
	void RemoveResource(unsigned int aSlot);
	void Clear();
private:
	friend class TextureAssetHandler;
	D3D11_VIEWPORT myViewport;
	ComPtr<ID3D11Texture2D> myTexture;
	ComPtr<ID3D11ShaderResourceView> mySRV;
	ComPtr<ID3D11RenderTargetView> myRTV;
};