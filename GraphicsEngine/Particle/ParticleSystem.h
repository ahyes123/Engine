#pragma once
#include "ParticleEmitter.h"
#include "SceneObject.h"

struct ParticleSystemTemplate
{
	ParticleEmitter Emitter;
	EmitterSettingsData Settings;
	Transform Transform;
};

class ParticleSystem : public SceneObject
{
	friend class ParticleAssetHandler;
	std::vector<ParticleEmitter> myEmitters;
	int myId;
	std::wstring myName = L"System";
public:
	void Update(float aDeltaTime);

	FORCEINLINE std::vector<ParticleEmitter>& GetEmitters() { return myEmitters; }

	std::wstring const& GetName() const { return myName; }
	void SetName(const std::wstring& aName) { myName = aName; }
	int const& GetId() const { return myId; }
	void SetId(const int& aId) { myId = aId; }
};

