#include "GraphicsEngine.pch.h"
#include "ShadowRenderer.h"

#include "DX11.h"
#include "Light/Light.h"
#include "Model/ModelInstance.h"
#include "Texture/TextureAssetHandler.h"

bool ShadowRenderer::Initialize()
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

	bufferDescription.ByteWidth = sizeof(Material::MaterialData);
	result = DX11::Device->CreateBuffer(&bufferDescription, nullptr, myMaterialBuffer.GetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ShadowRenderer::Render(const std::shared_ptr<Light>& aLight,
	const std::vector<std::shared_ptr<ModelInstance>>& aModelList)
{
	HRESULT result = S_FALSE;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	Light::LightBufferData lightData = aLight->GetLightBufferData();

	myFrameBufferData.View = lightData.LightView[0];
	myFrameBufferData.CamTranslation = lightData.Position;
	myFrameBufferData.Projection = lightData.LightProjection;
	myFrameBufferData.NearPlane = lightData.NearPlane;
	myFrameBufferData.FarPlane = lightData.FarPlane;
	myFrameBufferData.RenderMode = static_cast<unsigned int>(0);
	myFrameBufferData.DeltaTime = 0;
	myFrameBufferData.TotalTime = 0;

	DX11::Context->PSSetShader(nullptr, nullptr, 0);
	DX11::Context->GSSetShader(nullptr, nullptr, 0);

	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = DX11::Context->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

	if (FAILED(result))
	{

	}

	memcpy_s(bufferData.pData, sizeof(FrameBufferData), &myFrameBufferData, sizeof(FrameBufferData));

	DX11::Context->Unmap(myFrameBuffer.Get(), 0);
	DX11::Context->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());

	for(const std::shared_ptr<ModelInstance>& model : aModelList)
	{
		for (unsigned int m = 0; m < model->GetNumMeshes(); m++)
		{
			const Model::MeshData& meshData = model->GetMeshData(m);

			myObjectBufferData.World = model->GetTransform().GetMatrix();
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

			DX11::Context->VSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());

			DX11::Context->IASetVertexBuffers(0, 1, meshData.myVertexBuffer.GetAddressOf(), &meshData.myStride, &meshData.myOffset);
			DX11::Context->IASetIndexBuffer(meshData.myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			DX11::Context->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(meshData.myPrimitiveTopology));
			DX11::Context->IASetInputLayout(meshData.myInputLayout.Get());

			meshData.myMaterial->SetAsResource(myMaterialBuffer);
			DX11::Context->VSSetShader(meshData.myVertexShader.Get(), nullptr, 0);
			DX11::Context->PSSetShader(nullptr, nullptr, 0);

			DX11::Context->DrawIndexed(meshData.myNumberOfIndices, 0, 0);
		}
	}
}

void ShadowRenderer::RenderPoint(const std::shared_ptr<Light>& aLight,
	const std::vector<std::shared_ptr<ModelInstance>>& aModelList, int aIndex)
{
	HRESULT result = S_FALSE;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	Light::LightBufferData lightData = aLight->GetLightBufferData();

	myFrameBufferData.View = lightData.LightView[aIndex];
	myFrameBufferData.CamTranslation = lightData.Position;
	myFrameBufferData.Projection = lightData.LightProjection;
	myFrameBufferData.NearPlane = lightData.NearPlane;
	myFrameBufferData.FarPlane = lightData.FarPlane;
	myFrameBufferData.RenderMode = static_cast<unsigned int>(0);
	myFrameBufferData.DeltaTime = 0;
	myFrameBufferData.TotalTime = 0;

	DX11::Context->PSSetShader(nullptr, nullptr, 0);
	DX11::Context->GSSetShader(nullptr, nullptr, 0);

	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = DX11::Context->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

	if (FAILED(result))
	{

	}

	memcpy_s(bufferData.pData, sizeof(FrameBufferData), &myFrameBufferData, sizeof(FrameBufferData));

	DX11::Context->Unmap(myFrameBuffer.Get(), 0);
	DX11::Context->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());

	for (const std::shared_ptr<ModelInstance>& model : aModelList)
	{
		for (unsigned int m = 0; m < model->GetNumMeshes(); m++)
		{
			const Model::MeshData& meshData = model->GetMeshData(m);

			myObjectBufferData.World = model->GetTransform().GetMatrix();
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

			DX11::Context->VSSetConstantBuffers(1, 1, myObjectBuffer.GetAddressOf());

			DX11::Context->IASetVertexBuffers(0, 1, meshData.myVertexBuffer.GetAddressOf(), &meshData.myStride, &meshData.myOffset);
			DX11::Context->IASetIndexBuffer(meshData.myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			DX11::Context->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(meshData.myPrimitiveTopology));
			DX11::Context->IASetInputLayout(meshData.myInputLayout.Get());

			meshData.myMaterial->SetAsResource(myMaterialBuffer);
			DX11::Context->VSSetShader(meshData.myVertexShader.Get(), nullptr, 0);
			DX11::Context->PSSetShader(nullptr, nullptr, 0);

			DX11::Context->DrawIndexed(meshData.myNumberOfIndices, 0, 0);
		}
	}
}
