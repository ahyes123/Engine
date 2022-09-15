#pragma once
#include <array>
#include <wrl.h>

using namespace Microsoft::WRL;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11SamplerState;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

class DX11
{
	public:
		DX11() = default;
		~DX11() = default;

		static bool Initialize(HWND aWindowHandle, bool aEnableDeviceDebug);

		static void BeginFrame(std::array<float, 4> aClearColor);
		static void EndFrame();

		static void SetViewPort(float width, float height);

		static ComPtr<ID3D11Device> Device;
		static ComPtr<ID3D11DeviceContext> Context;
		static ComPtr<IDXGISwapChain> SwapChain;

		static ComPtr<ID3D11RenderTargetView> BackBuffer;
		static ComPtr<ID3D11DepthStencilView> DepthBuffer;

		static RECT ClientRect;
};

