#pragma once
#include <wrl.h>
#include "Matrix4x4.hpp"

class Light;
class ModelInstance;
class MeshComponent;
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

		CommonUtilities::Vector2<unsigned> Resolution;
		Vector2f padding;
		Vector4f FrustrumCorners[4];
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

	void Render(const std::shared_ptr<Light>& aLight, const std::vector<std::shared_ptr<MeshComponent>>& aModelList);
	void RenderPoint(const std::shared_ptr<Light>& aLight, const std::vector<std::shared_ptr<MeshComponent>>& aModelList, int aIndex);
};

