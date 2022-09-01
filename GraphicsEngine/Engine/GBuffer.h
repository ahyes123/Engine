#pragma once
#include <array>
#include <wrl/client.h>

using namespace Microsoft::WRL;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;

class GBuffer
{
	friend class TextureAssetHandler;

public:
	enum GBufferTexture
	{
		GB_ALBEDO,
		GB_NORMAL,
		GB_MATERIAL,
		GB_VERTEXNORMAL,
		GB_POSITION,
		GB_AMBIENTOCCLUSION,
		GB_COUNT
	};

	void Init();
	void SetAsTarget() const;
	void ClearTarget() const;
	void SetAsResource(unsigned int aStartSlot) const;
	void ClearResource(unsigned int aStartSlot) const;
	void Clear() const;

private:
	std::array<ComPtr<ID3D11RenderTargetView>, GBufferTexture::GB_COUNT> myRTVs;
	std::array<ComPtr<ID3D11ShaderResourceView>, GBufferTexture::GB_COUNT> mySRVs;
};

