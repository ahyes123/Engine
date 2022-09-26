#pragma once
#include <memory>

#include "DirectionalLight.h"
#include "EnvironmentLight.h"
#include "Light.h"
#include "PointLight.hpp"
#include "SpotLight.hpp"

class LightAssetHandler
{
	static inline std::vector<std::shared_ptr<Light>> myLights{};
	static inline std::shared_ptr<DirectionalLight> myDirectionalLight;
	static inline std::shared_ptr<EnvironmentLight> myEnvironmentLight;

	public:
		static void Update();
		static std::shared_ptr<DirectionalLight> CreateDirectionalLight(Vector3f aColor, float anIntensity, Vector3f aRotation, Vector3f aPosition);
		static std::shared_ptr<EnvironmentLight> CreateEnvironmentLight(const std::wstring& aTextureName);
		static std::shared_ptr<PointLight> CreatePointLight(Vector3f aColor, float anIntensity, float aRange, float aAttenuation, Vector3f aPosition);
		static std::shared_ptr<SpotLight> CreateSpotLight(Vector3f aColor, float anIntensity, float aRange, float aAttenuation, float aInnerCone, float aOuterCone, Vector3f aRotation, Vector3f aPosition);
};

