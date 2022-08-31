#pragma once
#include <unordered_map>
#include "ParticleEmitterTemplate.hpp"
#include "ParticleSystem.h"

class ParticleAssetHandler 
{
	typedef std::unordered_map<std::wstring, ParticleEmitterTemplate> EmitterTemplateMap;
	typedef std::unordered_map<std::wstring, ParticleSystemTemplate> SystemTemplateMap;

	inline static EmitterTemplateMap myEmitterTemplates{};
	inline static SystemTemplateMap mySystemTemplates{};
	static int id;
	public:
		static bool LoadSystemTemplate(const EmitterSettingsData& aSystemName);
		static bool LoadEmitterTemplate(const EmitterSettingsData& aSystemName);
		static std::shared_ptr<ParticleSystem> CreateParticleSystem(const EmitterSettingsData& aSystemName);
};

