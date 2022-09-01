#include "GraphicsEngine.pch.h"
#include "GBuffer.h"

#include "DX11.h"
#include "GraphicsEngine.h"

void GBuffer::Init()
{

}

void GBuffer::SetAsTarget() const
{
	ID3D11RenderTargetView* myRTVList[GBufferTexture::GB_COUNT];
	for (unsigned t = 0; t < myRTVs.size(); t++)
	{
		myRTVList[t] = myRTVs[t].Get();
	}
	DX11::Context->OMSetRenderTargets(GBufferTexture::GB_COUNT, &myRTVList[0], DX11::DepthBuffer.Get());
}

void GBuffer::ClearTarget() const
{
	ID3D11RenderTargetView* myRTVList[GBufferTexture::GB_COUNT];
	for (unsigned t = 0; t < myRTVs.size(); t++)
	{
		myRTVList[t] = nullptr;
	}
	DX11::Context->OMSetRenderTargets(GBufferTexture::GB_COUNT, &myRTVList[0], DX11::DepthBuffer.Get());
}

void GBuffer::SetAsResource(unsigned aStartSlot) const
{
	ID3D11ShaderResourceView* mySRVList[GBufferTexture::GB_COUNT];
	for (unsigned t = 0; t < mySRVs.size(); t++)
	{
		mySRVList[t] = mySRVs[t].Get();
	}

	DX11::Context->PSSetShaderResources(aStartSlot, GBufferTexture::GB_COUNT, &mySRVList[0]);
}

void GBuffer::ClearResource(unsigned aStartSlot) const
{
	DX11::Context->ClearDepthStencilView(DX11::DepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, aStartSlot);
}

void GBuffer::Clear() const
{
	std::array<FLOAT, 4> clearColor = {0, 0, 0, 0};
	ID3D11RenderTargetView* myRTVList[GBufferTexture::GB_COUNT];
	for (unsigned t = 0; t < myRTVs.size(); t++)
	{
		myRTVList[t] = myRTVs[t].Get();
	}
	DX11::Context->ClearRenderTargetView(myRTVList[0], &clearColor[0]);

	ClearTarget();
	ClearResource(0);
}
