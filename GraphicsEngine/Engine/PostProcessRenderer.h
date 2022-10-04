#pragma once
#include "Matrix4x4.hpp"
#include <wrl.h>

#include "Camera.h"

using namespace Microsoft::WRL;

class PostProcessRenderer
{
public:
	enum PostProcessPass
	{
		PP_Copy,
		PP_Luminance,
		PP_Gaussian,
		PP_Bloom,
		PP_SSAO,
		PP_Count
	};

	bool Initialize();
	void Render(PostProcessPass aPass);
	void Render(PostProcessPass aPass, std::shared_ptr<Camera> aCamera);

private:
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

	ComPtr<ID3D11Buffer> myFrameBuffer;
	ComPtr<ID3D11VertexShader> myFullscreenVS;

	std::array<ComPtr<ID3D11PixelShader>, PostProcessPass::PP_Count> myPassShader;
};

