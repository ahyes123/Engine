#include "GraphicsEngine.pch.h"
#include "Texture.h"
#include "DX11.h"

Texture::~Texture()
{
	myTexture.Reset();
	mySRV.Reset();
}

void Texture::SetAsResource(unsigned int aSlot)
{
	DX11::Context->PSSetShaderResources(aSlot, 1, mySRV.GetAddressOf());
}
