#pragma once
#include <wrl.h>

#include "ParticleEmitterTemplate.hpp"
#include "Vertex.hpp"
#include "Texture/Texture.h"

using namespace Microsoft::WRL;

class ParticleEmitter
{
	friend class ParticleAssetHandler;

	EmitterSettingsData myEmitterSettings;
	std::vector<ParticleVertex> myParticles;
	ComPtr<ID3D11Buffer> myVertexBuffer;
	UINT myStride;
	UINT myOffset;
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11GeometryShader> myGeometryShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	UINT myPrimitiveTopology;
	ComPtr<ID3D11InputLayout> myInputLayout;

	std::shared_ptr<Texture> myTexture;
	size_t myMaxNumberOfParticles;
	size_t myParticleCounter;
	float mySpawnRate;
	float myDuration;

	void InitParticle(size_t aParticleIndex);
	HRESULT CreateInputLayout(std::string* aVSData, ComPtr<ID3D11InputLayout>& outInputLayout);

public:
	virtual ~ParticleEmitter() = default;

	virtual bool Init(const ParticleEmitterTemplate& aTemplate);
	virtual void Update(float aDeltaTime);

	virtual void SetAsResource() const;
	virtual void Draw() const;

	FORCEINLINE EmitterSettingsData& GetEmitterSettings() { return myEmitterSettings; }

	void RefreshValues(const EmitterSettingsData aEmitterData);
	void RefreshSystem();
};

