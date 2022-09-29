#pragma once
#include "Matrix4x4.hpp"
#include <wrl.h>

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
		PP_Count
	};

	bool Initialize();
	void Render(PostProcessPass aPass);

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
	} myFrameBufferData;

	ComPtr<ID3D11Buffer> myFrameBuffer;
	ComPtr<ID3D11VertexShader> myFullscreenVS;

	std::array<ComPtr<ID3D11PixelShader>, PostProcessPass::PP_Count> myPassShader;
};

