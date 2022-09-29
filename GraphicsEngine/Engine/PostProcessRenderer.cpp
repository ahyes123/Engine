#include "GraphicsEngine.pch.h"
#include "PostProcessRenderer.h"

#include <fstream>

#include "DX11.h"

bool PostProcessRenderer::Initialize()
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

	std::ifstream vsFile;
	vsFile.open("Shaders/FullscreenVS.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	ID3D11VertexShader* vertexShader;
	result = DX11::Device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	vsFile.close();

	if (FAILED(result))
	{
		return false;
	}
	myFullscreenVS = vertexShader;

	std::array<std::string, PP_Count> shaderPaths;
	shaderPaths[PP_Copy] = "Shaders/PP_Copy.cso";
	shaderPaths[PP_Luminance] = "Shaders/PP_Luminance.cso";
	shaderPaths[PP_Gaussian] = "Shaders/PP_Gaussian.cso";
	shaderPaths[PP_Bloom] = "Shaders/PP_Bloom.cso";

	std::ifstream psFile;
	std::string psData;

	for (unsigned int i = 0; i < PP_Count; i++)
	{
		psFile.open(shaderPaths[i], std::ios::binary);
		psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		ID3D11PixelShader* pixelShader;
		result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, myPassShader[i].GetAddressOf());
		psFile.close();

		if (FAILED(result))
		{
			return false;
		}
	}

	return true;
}

void PostProcessRenderer::Render(PostProcessPass aPass)
{
	DX11::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX11::Context->IASetInputLayout(nullptr);
	DX11::Context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	DX11::Context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	DX11::Context->VSSetShader(myFullscreenVS.Get(), nullptr, 0);
	DX11::Context->PSSetShader(myPassShader[aPass].Get(), nullptr, 0);
	DX11::Context->GSSetShader(nullptr, nullptr, 0);
	DX11::Context->Draw(3, 0);
}
