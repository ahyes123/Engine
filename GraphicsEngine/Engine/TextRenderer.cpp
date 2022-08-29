#include "GraphicsEngine.pch.h"
#include "TextRenderer.h"
#include <fstream>
#include "DX11.h"
#include "ModelAssetHandler.h"

bool TextRenderer::Initialize()
{
	HRESULT result = S_FALSE;

	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufferDescription.ByteWidth = sizeof(FrameBufferData);
	result = DX11::Device->CreateBuffer(&bufferDescription, nullptr, myFrameBuffer.GetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	bufferDescription.ByteWidth = sizeof(ObjectBufferData);
	result = DX11::Device->CreateBuffer(&bufferDescription, nullptr, myObjectBuffer.GetAddressOf());

	if (FAILED(result))
	{
		return false;
	}


	std::ifstream vsFile;
	vsFile.open("Shaders/TextVS.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	ID3D11VertexShader* vertexShader;
	result = DX11::Device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	vsFile.close();

	if (FAILED(result))
	{
		return false;
	}

	std::ifstream psFile;
	psFile.open("Shaders/TextPS.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	ID3D11PixelShader* pixelShader;
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	psFile.close();

	if (FAILED(result))
	{
		return false;
	}

	result = ModelAssetHandler::CreateInputLayout(&vsData, myInputLayout);

	if(FAILED(result))
	{
		return false;
	}

	myVertexShader = vertexShader;
	myPixelShader = pixelShader;
	return true;
}

void TextRenderer::Render(const std::shared_ptr<Camera>& aCamera, const std::vector<std::shared_ptr<Text>>& someText)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	myFrameBufferData.myToCamera = Matrix4x4f::GetFastInverse(aCamera->GetTransform().GetMatrix());
	myFrameBufferData.myToProjection = aCamera->GetProjection();

	float camNearPlane = aCamera->GetNearPlane();
	float camFarPlane = aCamera->GetFarPlane();

	myFrameBufferData.myNearPlane = camNearPlane;
	myFrameBufferData.myFarPlane = camFarPlane;
	myFrameBufferData.myCameraPosition = { aCamera->GetTransform().GetPosition().x,
		aCamera->GetTransform().GetPosition().y,
		aCamera->GetTransform().GetPosition().z,
		1};
	myFrameBufferData.RenderMode = 0;

	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = DX11::Context->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	assert(SUCCEEDED(result));
	memcpy_s(bufferData.pData, sizeof(FrameBufferData), &myFrameBufferData, sizeof(FrameBufferData));
	DX11::Context->Unmap(myFrameBuffer.Get(), 0);

	DX11::Context->PSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
	DX11::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX11::Context->IASetInputLayout(myInputLayout.Get());
	DX11::Context->GSSetShader(nullptr, nullptr, 0);
	DX11::Context->VSSetShader(myVertexShader.Get(), nullptr, 0);
	DX11::Context->PSSetShader(myPixelShader.Get(), nullptr, 0);

	for(auto& text : someText)
	{
		Text::TextData textData = text->GetTextData();

		myObjectBufferData.World = text->GetTransform().GetMatrix();
		myObjectBufferData.is2D = text->GetIs2D();

		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = DX11::Context->Map(myObjectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			
		}

		memcpy_s(bufferData.pData, sizeof(ObjectBufferData), &myObjectBufferData, sizeof(ObjectBufferData));
		DX11::Context->Unmap(myObjectBuffer.Get(), 0);

		DX11::Context->VSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
		DX11::Context->IASetVertexBuffers(0, 1, textData.myVertexBuffer.GetAddressOf(), &textData.myStride, &textData.myOffset);
		DX11::Context->IASetIndexBuffer(textData.myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DX11::Context->PSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
		DX11::Context->PSSetShaderResources(0, 1, text->GetFont()->SRV.GetAddressOf());

		DX11::Context->DrawIndexed(textData.myNumber0fIndices, 0, 0);
	}
}
