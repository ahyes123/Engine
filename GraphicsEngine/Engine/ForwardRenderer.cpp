#include "GraphicsEngine.pch.h"
#include "ForwardRenderer.h"
#include "Light/DirectionalLight.h"
#include "Light/EnvironmentLight.h"
#include "Light/Light.h"
#include "GraphicsEngine.h"

bool ForwardRenderer::Initialize()
{
	HRESULT result = S_FALSE;

	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufferDescription.ByteWidth = sizeof(FrameBufferData);
	result = DX11::Device->CreateBuffer(&bufferDescription, nullptr, myFrameBuffer.GetAddressOf());

	if(FAILED(result))
	{
		return false;
	}

	bufferDescription.ByteWidth = sizeof(ObjectBufferData);
	result = DX11::Device->CreateBuffer(&bufferDescription, nullptr, myObjectBuffer.GetAddressOf());

	if(FAILED(result))
	{
		return false;
	}

	bufferDescription.ByteWidth = sizeof(Material::MaterialData);
	result = DX11::Device->CreateBuffer(&bufferDescription, nullptr, myMaterialBuffer.GetAddressOf());

	if(FAILED(result))
	{
		return false;
	}

	bufferDescription.ByteWidth = sizeof(SceneLightBuffer);
	result = DX11::Device->CreateBuffer(&bufferDescription, nullptr, myLightBuffer.GetAddressOf());

	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void ForwardRenderer::RenderModels(const std::shared_ptr<Camera>& aCamera, const std::vector<std::shared_ptr<ModelInstance>>& aModelList,
	const std::shared_ptr<DirectionalLight>& aDirectionalLight, const std::vector<std::shared_ptr<Light>>& aLightList, const std::shared_ptr<EnvironmentLight>& anEnvironmentLight)
{
	HRESULT result = S_FALSE;

	D3D11_MAPPED_SUBRESOURCE bufferData;

	myFrameBufferData.View = Matrix4x4f::GetFastInverse(aCamera->GetTransform().GetMatrix());
	myFrameBufferData.CamTranslation = aCamera->GetTransform().GetPosition();
	myFrameBufferData.Projection = aCamera->GetProjection();
	myFrameBufferData.RenderMode = static_cast<unsigned int>(GraphicsEngine::GetRenderMode());

	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = DX11::Context->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

	if (FAILED(result))
	{

	}

	memcpy_s(bufferData.pData, sizeof(FrameBufferData), &myFrameBufferData, sizeof(FrameBufferData));

	DX11::Context->Unmap(myFrameBuffer.Get(), 0);
	DX11::Context->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
	DX11::Context->PSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());

	if (aDirectionalLight)
	{
		mySceneLightBufferData.DirectionalLight = aDirectionalLight->GetLightBufferData();
	}

	if (anEnvironmentLight)
	{
		anEnvironmentLight->SetAsResource(nullptr);
	}

	mySceneLightBufferData.NumLights = 0;
	ZeroMemory(mySceneLightBufferData.Lights, sizeof(Light::LightBufferData) * MAX_FORWARD_LIGHTS);
	for(size_t l = 0; l < aLightList.size() && l < MAX_FORWARD_LIGHTS; l++)
	{
		mySceneLightBufferData.Lights[l] = aLightList[l]->GetLightBufferData();
		mySceneLightBufferData.NumLights++;
	}

	for (const std::shared_ptr<ModelInstance>& model : aModelList)
	{
		for (int i = 0; i < model->GetNumMeshes(); i++)
		{
			const Model::MeshData& meshData = model->GetMeshData(i);

			//myObjectBufferData.World = model->GetTransform().GetMatrix();
			myObjectBufferData.HasBones = model->HasBones();

			if (myObjectBufferData.HasBones)
			{
				memcpy_s(&myObjectBufferData.BoneData[0], sizeof(Matrix4x4f) * 128, model->myBoneTransforms, sizeof(Matrix4x4f) * 128);
			}

			ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));

			result = DX11::Context->Map(myObjectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

			if (FAILED(result))
			{

			}

			memcpy_s(bufferData.pData, sizeof(ObjectBufferData), &myObjectBufferData, sizeof(ObjectBufferData));

			DX11::Context->Unmap(myObjectBuffer.Get(), 0);

			if (meshData.myMaterial)
			{
				meshData.myMaterial->SetAsResource(myMaterialBuffer);
			}

			DX11::Context->IASetVertexBuffers(0, 1, meshData.myVertexBuffer.GetAddressOf(), &meshData.myStride, &meshData.myOffset);
			DX11::Context->IASetIndexBuffer(meshData.myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			DX11::Context->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(meshData.myPrimitiveTopology));
			DX11::Context->IASetInputLayout(meshData.myInputLayout.Get());
			DX11::Context->VSSetShader(meshData.myVertexShader.Get(), nullptr, 0);
			DX11::Context->GSSetShader(nullptr, nullptr, 0);
			DX11::Context->PSSetShader(meshData.myPixelShader.Get(), nullptr, 0);
			DX11::Context->VSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
			DX11::Context->PSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
			DX11::Context->PSSetConstantBuffers(2, 1, myMaterialBuffer.GetAddressOf());
			DX11::Context->DrawIndexed(meshData.myNumberOfIndices, 0, 0);
		}
	}
}

void ForwardRenderer::RenderParticles(const std::shared_ptr<Camera>& aCamera,
	const std::vector<std::shared_ptr<ParticleSystem>>& aParticleList)
{
	HRESULT result = S_FALSE;

	D3D11_MAPPED_SUBRESOURCE bufferData;

	myFrameBufferData.View = Matrix4x4f::GetFastInverse(aCamera->GetTransform().GetMatrix());
	myFrameBufferData.Projection = aCamera->GetProjection();
	myFrameBufferData.RenderMode = static_cast<unsigned int>(GraphicsEngine::GetRenderMode());

	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = DX11::Context->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

	if (FAILED(result))
	{

	}

	memcpy_s(bufferData.pData, sizeof(FrameBufferData), &myFrameBufferData, sizeof(FrameBufferData));

	DX11::Context->Unmap(myFrameBuffer.Get(), 0);
	DX11::Context->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
	DX11::Context->GSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
	DX11::Context->PSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());

	for(const std::shared_ptr<ParticleSystem>& system : aParticleList)
	{
		myObjectBufferData.World = system->GetTransform().GetMatrix();
		myObjectBufferData.HasBones = false;

		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));

		result = DX11::Context->Map(myObjectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

		if (FAILED(result))
		{

		}

		memcpy_s(bufferData.pData, sizeof(ObjectBufferData), &myObjectBufferData, sizeof(ObjectBufferData));

		DX11::Context->Unmap(myObjectBuffer.Get(), 0);

		DX11::Context->VSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
		DX11::Context->GSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());
		DX11::Context->PSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());

		for (const ParticleEmitter& emitter : system->GetEmitters())
		{
			emitter.SetAsResource();
			emitter.Draw();
		}
	}
}
