#pragma once
#include <memory>
#include <vector>
#include "Math/Matrix4x4.hpp"
#include <wrl.h>
#include "Scene/Camera.h"
#include <d3d11.h>
#include <memory>

#include "Light/DirectionalLight.h"
#include "Light/EnvironmentLight.h"
#include "Model/ModelInstance.h"
#include "Particle/ParticleSystem.h"

using namespace Microsoft::WRL;

class ForwardRenderer
{
	struct FrameBufferData
	{
		Matrix4x4f View;
		Matrix4x4f Projection;
		Vector3f CamTranslation;
		unsigned int RenderMode;
	} myFrameBufferData;

	struct ObjectBufferData
	{
		Matrix4x4f World;
		bool HasBones;
		Vector3f padding;
		Matrix4x4f BoneData[128];
	} myObjectBufferData;

	ComPtr<ID3D11Buffer> myFrameBuffer;
	ComPtr<ID3D11Buffer> myObjectBuffer;
	ComPtr<ID3D11Buffer> myMaterialBuffer;
	ComPtr<ID3D11Buffer> myLightBuffer;

	public:
		bool Initialize();
		void RenderModels(const std::shared_ptr<Camera>& aCamera, const std::vector<std::shared_ptr<ModelInstance>>& aModelList, const std::
					shared_ptr<DirectionalLight>& aDirectionalLight, const std::shared_ptr<EnvironmentLight>& anEnvironmentLight);
		void RenderParticles(const std::shared_ptr<Camera>& aCamera, const std::vector<std::shared_ptr<ParticleSystem>>& aParticleList);
};
