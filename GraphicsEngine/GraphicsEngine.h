#pragma once

#include "DeferredRenderer.h"
#include "PostProcessRenderer.h"
#include "RenderTarget.h"
#include "ShadowRenderer.h"
#include "Engine/TextRenderer.h"
#include "Engine/ForwardRenderer.h"
#include "Scene/Scene.h"
#include "Model/ModelAssetHandler.h"
#include <filesystem>

#include "Components/MeshComponent.h"

using std::filesystem::directory_iterator;

enum class RenderMode : unsigned int
{
	Default,
	UV1,
	VertexColor,
	VertexNormal,
	PixelNormal,
	AlbedoMap,
	NormalMap,
	DiffuseLight,
	AmbientLight,
	DirectionalOnly,
	AmbientOnly,
	AmbientOcclusion,
	Roughness,
	Metalness,
	Emissivieness,
	COUNT
};

class GraphicsEngine
{
	SIZE myWindowSize{ 0,0 };

	static HWND myWindowHandle;
	static std::shared_ptr<Camera> myCamera;

	ForwardRenderer myForwardRenderer;
	DeferredRenderer myDeferredRenderer;
	ShadowRenderer myShadowRenderer;
	TextRenderer myTextRenderer;
	ModelAssetHandler myModelAssetHandler;

	std::shared_ptr<DirectionalLight> myDirectionalLight;
	std::shared_ptr<EnvironmentLight> myEnvironmentLight;
	std::vector<std::shared_ptr<Light>> myLights;

	static RenderMode myRenderMode;
	static RECT windowRect;
	// Container window message pump.
	static LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

	static std::array<FLOAT, 4> ourClearColor;
	static bool myAutoSave;
	static bool myFileExists;

	std::unique_ptr<RenderTarget> myIntermediateTargetB;
	std::unique_ptr<RenderTarget> myHalfSizeTarget;
	std::unique_ptr<RenderTarget> myQuarterSizeTarget;
	std::unique_ptr<RenderTarget> myBlurTargetA;
	std::unique_ptr<RenderTarget> myBlurTargetB;

	std::shared_ptr<Texture> myNoiseTexture;

	PostProcessRenderer myPPRenderer;

public:
	static inline bool enableSSAO = true;
	static inline std::unique_ptr<RenderTarget> mySSAOTarget;
	static std::unique_ptr<RenderTarget> myIntermediateTargetA;

	static bool myClearColorBlending;
	static float myClearColorBlendFactor;
	static std::array<std::array<FLOAT, 4>, 2> myClearColorPresets;
	static std::string myCurrentClearColorPreset;

	bool Initialize(unsigned someX, unsigned someY, unsigned someWidth, unsigned someHeight, bool enableDeviceDebug);

	static void DragDrop(WPARAM aWparam);
	static bool HasFile(const std::string& aFile, const std::string aFileExtension);
	static void AddMissingFile(const std::string& aFile, const std::string aFileExtension);
	static void AddAssets(std::filesystem::path aFilePath);

	void BeginFrame();
	void EndFrame();
	void RenderFrame();

	static void SetRenderMode(RenderMode aMode);
	static RenderMode GetRenderMode();

	static RECT GetWindowRectangle();

	static std::shared_ptr<Camera> GetCamera() { return myCamera; }
	static std::array<FLOAT, 4> &GetClearColor() { return ourClearColor; }

	static bool& GetAutoSave() { return myAutoSave; }

	[[nodiscard]] static HWND FORCEINLINE GetWindowHandle() { return myWindowHandle; }
	//[[nodiscard]] SIZE FORCEINLINE GetWindowSize() const { return myWindowSize; }
};