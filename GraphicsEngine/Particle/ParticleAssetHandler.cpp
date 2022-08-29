#include "GraphicsEngine.pch.h"
#include "ParticleAssetHandler.h"
#include <fstream>
#include "json.hpp"
#include "../Scene/SceneHandler.h"

bool ParticleAssetHandler::LoadSystemTemplate(const std::wstring& someFilePath)
{
	if (const auto It = mySystemTemplates.find(someFilePath); It == mySystemTemplates.end())
	{
		LoadEmitterTemplate(someFilePath);
		ParticleEmitterTemplate emitterTemplate = myEmitterTemplates[someFilePath];
		ParticleEmitter emitter;
		emitter.Init(emitterTemplate);

		ParticleSystemTemplate systemTemplate;
		systemTemplate.Settings = emitterTemplate.EmitterSettings;
		systemTemplate.Emitter = emitter;
		systemTemplate.Transform.SetPosition({ 0, 0, 0 });

		mySystemTemplates.insert({ someFilePath, systemTemplate });
	}

	return true;
}

bool ParticleAssetHandler::LoadEmitterTemplate(const std::wstring& someFilePath)
{
	if (const auto It = myEmitterTemplates.find(someFilePath); It == myEmitterTemplates.end())
	{
		std::ifstream particleStream(someFilePath);
		using nlohmann::json;
		json particleJSON;
		particleStream >> particleJSON;
		particleStream.close();

		EmitterSettingsData data;
		data.SpawnRate = particleJSON["EmitterSettings"]["SpawnRate"];
		data.LifeTime = particleJSON["EmitterSettings"]["LifeTime"];

		data.StartVelocity.x = particleJSON["EmitterSettings"]["StartVelocity"]["x"];
		data.StartVelocity.y = particleJSON["EmitterSettings"]["StartVelocity"]["y"];
		data.StartVelocity.z = particleJSON["EmitterSettings"]["StartVelocity"]["z"];

		data.EndVelocity.x = particleJSON["EmitterSettings"]["EndVelocity"]["x"];
		data.EndVelocity.y = particleJSON["EmitterSettings"]["EndVelocity"]["y"];
		data.EndVelocity.z = particleJSON["EmitterSettings"]["EndVelocity"]["z"];

		data.GravityScale = particleJSON["EmitterSettings"]["GravityScale"];
		data.StartSize = particleJSON["EmitterSettings"]["StartSize"];
		data.EndSize = particleJSON["EmitterSettings"]["EndSize"];

		data.StartColor.x = particleJSON["EmitterSettings"]["StartColor"]["x"];
		data.StartColor.y = particleJSON["EmitterSettings"]["StartColor"]["y"];
		data.StartColor.z = particleJSON["EmitterSettings"]["StartColor"]["z"];
		data.StartColor.w = particleJSON["EmitterSettings"]["StartColor"]["w"];

		data.EndColor.x = particleJSON["EmitterSettings"]["EndColor"]["x"];
		data.EndColor.y = particleJSON["EmitterSettings"]["EndColor"]["y"];
		data.EndColor.z = particleJSON["EmitterSettings"]["EndColor"]["z"];
		data.EndColor.w = particleJSON["EmitterSettings"]["EndColor"]["w"];

		data.Looping = particleJSON["EmitterSettings"]["Looping"];
		data.HasDuration = particleJSON["EmitterSettings"]["HasDuration"];
		data.Duration = particleJSON["EmitterSettings"]["Duration"];

		ParticleEmitterTemplate emitterTemplate;
		emitterTemplate.EmitterSettings = data;
		emitterTemplate.Path = someFilePath;

		myEmitterTemplates.insert({ someFilePath, emitterTemplate });
	}
	return true;
}

std::shared_ptr<ParticleSystem> ParticleAssetHandler::CreateParticleSystem(const std::wstring& aSystemName)
{
	std::shared_ptr<ParticleSystem> system = std::make_shared<ParticleSystem>();
	LoadSystemTemplate(aSystemName);
	system->myEmitters.push_back(mySystemTemplates[aSystemName].Emitter);
	system->SetId(SceneHandler::GetActiveScene()->GetNextId());
	return system;
}