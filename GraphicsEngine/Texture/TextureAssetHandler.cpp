#include "GraphicsEngine.pch.h"
#include "TextureAssetHandler.h"
#include "DDSTextureLoader11.h"
#include "DX11.h"
#include "Texture.h"

std::shared_ptr<Texture> TextureAssetHandler::GetTexture(const std::wstring& aName)
{
	return myRegistry[aName];
}

bool TextureAssetHandler::LoadTexture(const std::wstring& aFileName)
{
	if (const auto It = myRegistry.find(aFileName); It == myRegistry.end())
	{
		Texture result;
		result.myName = aFileName;
		const HRESULT createResult = DirectX::CreateDDSTextureFromFile(DX11::Device.Get(), aFileName.c_str(), result.myTexture.GetAddressOf(), result.mySRV.GetAddressOf());

		if (SUCCEEDED(createResult))
		{
			myRegistry.insert({ aFileName, std::make_shared<Texture>(result) });
			return true;
		}

	}
	else if (myRegistry[aFileName].get() != nullptr)
	{
		return true;
	}
	return false;
}