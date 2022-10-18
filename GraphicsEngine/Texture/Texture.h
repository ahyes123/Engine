#pragma once
#include <wrl/client.h>

using namespace Microsoft::WRL;

struct ID3D11Resource;
struct ID3D11ShaderResourceView;

class Texture
{
	friend class TextureAssetHandler;

	protected:
		std::wstring myName;
		ComPtr<ID3D11Resource> myTexture;
		ComPtr<ID3D11ShaderResourceView> mySRV;

	public:
		virtual ~Texture();

		void SetAsResource(unsigned int aSlot);

		ComPtr<ID3D11ShaderResourceView> GetSRV() { return mySRV; }
};

