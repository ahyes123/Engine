#include "GraphicsEngine.pch.h"
#include "ParticleAssetHandler.h"
#include <fstream>
#include "../External/nloman/json.hpp"
#include "../Scene/SceneHandler.h"

int ParticleAssetHandler::id;

bool ParticleAssetHandler::LoadSystemTemplate(const EmitterSettingsData& aSystemName)
{
	LoadEmitterTemplate(aSystemName);
	ParticleEmitterTemplate emitterTemplate = myEmitterTemplates[std::to_wstring(id)];
	ParticleEmitter emitter;
	emitter.Init(emitterTemplate);

	ParticleSystemTemplate systemTemplate;
	systemTemplate.Settings = emitterTemplate.EmitterSettings;
	systemTemplate.Emitter = emitter;

	mySystemTemplates.insert({ std::to_wstring(id), systemTemplate});
	return true;
}

bool ParticleAssetHandler::LoadEmitterTemplate(const EmitterSettingsData& aSystemName)
{
	ParticleEmitterTemplate emitterTemplate;
	emitterTemplate.EmitterSettings = aSystemName;
	emitterTemplate.Path = std::to_wstring(id);

	myEmitterTemplates.insert({ std::to_wstring(id), emitterTemplate });
	return true;
}

std::shared_ptr<ParticleSystem> ParticleAssetHandler::CreateParticleSystem(const EmitterSettingsData& aSystemName)
{
	std::shared_ptr<ParticleSystem> system = std::make_shared<ParticleSystem>();
	LoadSystemTemplate(aSystemName);
	system->myEmitters.push_back(mySystemTemplates[std::to_wstring(id)].Emitter);
	system->SetId(SceneHandler::GetActiveScene()->GetNextId());
	id++;
	return system;
}