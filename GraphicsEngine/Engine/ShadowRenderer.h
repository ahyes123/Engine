#pragma once
#include <wrl.h>
#include <d3d11.h>
#include "Matrix4x4.hpp"

class Light;
class ModelInstance;
using namespace Microsoft::WRL;


class ShadowRenderer
{
	struct FrameBufferData
	{
		Matrix4x4f View;
		Matrix4x4f Projection;
		Vector3f CamTranslation;
		unsigned int RenderMode;

		float NearPlane;
		float FarPlane;
		float DeltaTime;
		float TotalTime;
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

public:
	bool Initialize();

	void Render(const std::shared_ptr<Light>& aLight, const std::vector<std::shared_ptr<ModelInstance>>& aModelList);
	void RenderPoint(const std::shared_ptr<Light>& aLight, const std::vector<std::shared_ptr<ModelInstance>>& aModelList, int aIndex);
};

