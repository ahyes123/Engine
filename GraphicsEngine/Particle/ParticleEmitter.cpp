#include "GraphicsEngine.pch.h"
#include "ParticleEmitter.h"

#include <fstream>
#include <iostream>

#include "DX11.h"
#include "UtilityFunctions.hpp"
#include "Texture/TextureAssetHandler.h"
#include "../Tools/InputHandler.h"
#include "../Scene/SceneHandler.h"

void ParticleEmitter::InitParticle(size_t aParticleIndex)
{
	size_t i = aParticleIndex;
	myParticles[i].Position = { static_cast<float>(rand() % 100), 0, 100, 1 };
	myParticles[i].Color = myEmitterSettings.StartColor;
	myParticles[i].LifeTime = 0;
	myParticles[i].Scale = { myEmitterSettings.StartSize, myEmitterSettings.StartSize, myEmitterSettings.StartSize };
	myParticles[i].Velocity = myEmitterSettings.StartVelocity;
}

HRESULT ParticleEmitter::CreateInputLayout(std::string* aVSData, ComPtr<ID3D11InputLayout>& outInputLayout)
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SCALE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"LIFETIME", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	return DX11::Device->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), aVSData->data(), aVSData->size(), outInputLayout.GetAddressOf());
}

bool ParticleEmitter::Init(const ParticleEmitterTemplate& aTemplate)
{
	myEmitterSettings = aTemplate.EmitterSettings;
	myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	myMaxNumberOfParticles = static_cast<size_t>(ceilf(myEmitterSettings.SpawnRate * myEmitterSettings.LifeTime));

	myParticles.resize(myMaxNumberOfParticles);

	myParticleCounter = 0;
	mySpawnRate = 0;

	HRESULT result;

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(myParticles.size()) * static_cast<UINT>(sizeof(ParticleVertex));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	vertexSubresourceData.pSysMem = &myParticles[0];

	result = DX11::Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, myVertexBuffer.GetAddressOf());

	if (FAILED(result))
	{
		return false;
	}

	myStride = sizeof(ParticleVertex);

	std::ifstream vsFile;
	vsFile.open("Shaders/ParticleVS.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	result = DX11::Device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, myVertexShader.GetAddressOf());
	vsFile.close();

	if (FAILED(result))
	{
		return false;
	}

	std::ifstream gsFile;
	gsFile.open("Shaders/ParticleGS.cso", std::ios::binary);
	std::string gsData = { std::istreambuf_iterator<char>(gsFile), std::istreambuf_iterator<char>() };
	result = DX11::Device->CreateGeometryShader(gsData.data(), gsData.size(), nullptr, myGeometryShader.GetAddressOf());
	gsFile.close();

	if (FAILED(result))
	{
		return false;
	}

	std::ifstream psFile;
	psFile.open("Shaders/ParticlePS.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, myPixelShader.GetAddressOf());
	psFile.close();

	if (FAILED(result))
	{
		return false;
	}

	result = CreateInputLayout(&vsData, myInputLayout);

	if (FAILED(result))
	{
		return false;
	}

	if (!TextureAssetHandler::LoadTexture(L"Sprites/ParticleStar.dds"))
	{
		return false;
	}

	myTexture = TextureAssetHandler::GetTexture(L"Sprites/ParticleStar.dds");

	return true;
}

void ParticleEmitter::Update(float aDeltaTime)
{
	mySpawnRate += aDeltaTime;
	if (mySpawnRate >= 1 / myEmitterSettings.SpawnRate && myParticleCounter < myMaxNumberOfParticles)
	{
		mySpawnRate = 0;
		InitParticle(myParticleCounter);
		myParticleCounter++;
	}
	else if(myParticleCounter > myMaxNumberOfParticles)
	{
		myParticleCounter = myMaxNumberOfParticles;
	}
	for (size_t p = 0; p < myParticleCounter; p++)
	{
		ParticleVertex& particle = myParticles[p];
		particle.LifeTime += aDeltaTime;

		if (myEmitterSettings.Looping)
		{
			if (particle.LifeTime > myEmitterSettings.LifeTime)
			{
				InitParticle(p);
			}
		}
		Vector4f velocity = { particle.Velocity.x, particle.Velocity.y - myEmitterSettings.GravityScale, particle.Velocity.z, 0 };
		particle.Position += velocity * aDeltaTime;
		particle.Color = CommonUtilities::Lerp(myEmitterSettings.StartColor, myEmitterSettings.EndColor, particle.LifeTime / myEmitterSettings.LifeTime);
		particle.Scale = CommonUtilities::Lerp(myEmitterSettings.StartSize, myEmitterSettings.EndSize, particle.LifeTime / myEmitterSettings.LifeTime);
		particle.Velocity = CommonUtilities::Lerp(myEmitterSettings.StartVelocity, myEmitterSettings.EndVelocity, particle.LifeTime / myEmitterSettings.LifeTime);
	}
	if (myEmitterSettings.HasDuration)
	{
		myDuration += aDeltaTime;
		if (myDuration >= myEmitterSettings.Duration)
		{
			std::shared_ptr<Scene> curScene = SceneHandler::GetActiveScene();
			for (int i = 0; i < curScene->GetParticleSystems().size(); i++)
			{
				if (this == &curScene->GetParticleSystems()[i].get()->GetEmitters()[0])
				{
					curScene->RemoveParticleSystem(curScene->GetParticleSystems()[i]);
				}
			}
		}
	}
}

void ParticleEmitter::SetAsResource() const
{
	HRESULT result = S_FALSE;

	D3D11_MAPPED_SUBRESOURCE bufferData;
	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = DX11::Context->Map(myVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);

	if (FAILED(result))
	{
		std::cout << "Particle System Failed To Set Resource" << std::endl;
	}

	memcpy_s(bufferData.pData, sizeof(ParticleVertex) * myParticles.size(), &myParticles[0], sizeof(ParticleVertex) * myParticles.size());

	DX11::Context->Unmap(myVertexBuffer.Get(), 0);

	DX11::Context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &myStride, &myOffset);
	DX11::Context->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(myPrimitiveTopology));
	DX11::Context->IASetInputLayout(myInputLayout.Get());

	DX11::Context->VSSetShader(myVertexShader.Get(), nullptr, 0);
	DX11::Context->GSSetShader(myGeometryShader.Get(), nullptr, 0);
	DX11::Context->PSSetShader(myPixelShader.Get(), nullptr, 0);

	if (myTexture)
	{
		myTexture->SetAsResource(0);
	}
}

void ParticleEmitter::Draw() const
{
	DX11::Context->Draw(static_cast<UINT>(myParticles.size()), 0);
}

void ParticleEmitter::RefreshValues(const EmitterSettingsData aEmitterData)
{
	myEmitterSettings = aEmitterData;
	myMaxNumberOfParticles = static_cast<size_t>(ceilf(myEmitterSettings.SpawnRate * myEmitterSettings.LifeTime));

	myParticles.resize(myMaxNumberOfParticles);

	HRESULT result;

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(myParticles.size()) * static_cast<UINT>(sizeof(ParticleVertex));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	vertexSubresourceData.pSysMem = &myParticles[0];

	result = DX11::Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, myVertexBuffer.GetAddressOf());

	if (FAILED(result))
	{
		std::cout << "FATAL ERROR IN PARTICLE EMITTER" << std::endl;
	}
}

void ParticleEmitter::RefreshSystem()
{
	ParticleEmitterTemplate temp;
	temp.EmitterSettings = myEmitterSettings;
	Init(temp);
}