#pragma once
#include <wrl.h>

#include "Math/Matrix4x4.hpp"
#include "Scene/Camera.h"
#include "Text/Text.h"

using namespace Microsoft::WRL;

class TextRenderer
{
	struct FrameBufferData
	{
		Matrix4x4f myToCamera;
		Matrix4x4f myToProjection;
		Vector4f myCameraPosition;
		float myNearPlane;
		float myFarPlane;
		unsigned int RenderMode;
		float padding;
	} myFrameBufferData;

	struct ObjectBufferData
	{
		Matrix4x4f World;
		bool is2D;
		Vector3f padding;
	} myObjectBufferData;

	ComPtr<ID3D11Buffer> myFrameBuffer;
	ComPtr<ID3D11Buffer> myObjectBuffer;

	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	ComPtr<ID3D11InputLayout> myInputLayout;

public:
	bool Initialize();
	void Render(const std::shared_ptr<Camera>& aCamera, const std::vector<std::shared_ptr<Text>>& someText);
};

