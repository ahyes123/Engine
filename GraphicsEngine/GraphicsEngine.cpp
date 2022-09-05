#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"
#include <iostream>
#include "DX11.h"
#include "InputHandler.h"
#include "Timer.h"
#include "Light/LightAssetHandler.h"
#include "Text/TextFactory.h"
#include "RenderStateManager.h"
#include "Particle/ParticleAssetHandler.h"
#include "Editor/EditorInterface.h"
#include "Editor/Editor.h"
#include "Scene/SceneHandler.h"
#include <filesystem>
#include "Engine/ComponentHandler.h"
#include "../GraphicsEngine/Engine/ComponentHandler.h"

using namespace CommonUtilities;

RenderMode GraphicsEngine::myRenderMode;
RECT GraphicsEngine::windowRect;
HWND GraphicsEngine::myWindowHandle;
std::shared_ptr<Camera> GraphicsEngine::myCamera;
std::array<FLOAT, 4> GraphicsEngine::ourClearColor;

bool GraphicsEngine::Initialize(unsigned someX, unsigned someY,
	unsigned someWidth, unsigned someHeight,
	bool enableDeviceDebug)
{
	// Initialize our window:
	WNDCLASS windowClass = {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = GraphicsEngine::WinProc;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = L"TGP";
	RegisterClass(&windowClass);

	myWindowHandle = CreateWindow(
		L"TGP",
		L"Sexy_Engine",
		WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE,
		someX,
		someY,
		someWidth,
		someHeight,
		nullptr, nullptr, nullptr,
		this
	);

	ourClearColor = { 0.1f, 0.1f, 0.1f, 1 };

	if (!DX11::Initialize(myWindowHandle, enableDeviceDebug))
		return false;

	TextFactory::Init();
	RenderStateManager::Initialize();
	myCamera = std::make_shared<Camera>();
	myCamera->Init(90, { 1920, 1080 }, 0.1f, 15000.f);
	myCamera->SetPosition(0, 0, -300.f);
	myCamera->SetRotation(0, 0, 0);

	SceneHandler::LoadAllScenes();
	Editor::LoadSettings();
	ComponentHandler::Init();

	myDirectionalLight = LightAssetHandler::CreateDirectionalLight({ 1, 1, 1 }, 1, { 45, -45, 0 });
	myEnvironmentLight = LightAssetHandler::CreateEnvironmentLight(L"skansen_cubemap.dds");

	/*for (size_t i = 0; i < SceneHandler::GetScenes().size(); i++)
	{
		SceneHandler::GetScenes()[i]->AddParticleSystem(ParticleAssetHandler::CreateParticleSystem(L"Json/BaseParticleSystem.json"));
		SceneHandler::GetScenes()[i]->GetParticleSystems()[0]->SetPosition(0, 0, 100);
		SceneHandler::GetScenes()[i]->AddParticleSystem(ParticleAssetHandler::CreateParticleSystem(L"Json/BaseParticleSystem.json"));
		SceneHandler::GetScenes()[i]->GetParticleSystems()[1]->SetPosition(300, 0, 100);
	}*/
	myRenderMode = RenderMode::Default;
	GetWindowRect(myWindowHandle, &windowRect);

	if (!myForwardRenderer.Initialize())
		return false;

	if (!myDeferredRenderer.Initialize())
		return false;

	if (!myTextRenderer.Initialize())
		return false;

	return true;
}

LRESULT CALLBACK GraphicsEngine::WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	// We want to be able to access the Graphics Engine instance from inside this function.
	static GraphicsEngine* graphicsEnginePtr = nullptr;

	if (uMsg == WM_DESTROY || uMsg == WM_CLOSE)
	{
#ifdef _DEBUG
		Editor::SaveScenes();
		Editor::SaveSettings();
#endif // _DEBUG
		PostQuitMessage(0);
	}
	else if (uMsg == WM_CREATE)
	{
		const CREATESTRUCT* createdStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
		graphicsEnginePtr = static_cast<GraphicsEngine*>(createdStruct->lpCreateParams);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void GraphicsEngine::BeginFrame()
{
	// F1 - This is where we clear our buffers and start the DX frame.

	DX11::BeginFrame(ourClearColor);
}

void GraphicsEngine::RenderFrame()
{
	GetWindowRect(myWindowHandle, &windowRect);
	myCamera->Update(Timer::GetDeltaTime());
	SceneHandler::UpdateCurrentScene();
	const std::shared_ptr<Camera> camera = SceneHandler::GetActiveScene()->GetCamera();
#ifdef _DEBUG
	EditorInterface::ShowEditor();

	if (InputHandler::GetKeyIsPressed(VK_F6))
	{
		unsigned int currentRenderMode = static_cast<unsigned int>(myRenderMode);
		currentRenderMode++;
		if (currentRenderMode == static_cast<unsigned char>(RenderMode::COUNT))
		{
			currentRenderMode = 0;
		}

		myRenderMode = static_cast<RenderMode>(currentRenderMode);
	}
	if (InputHandler::GetKeyIsPressed(VK_F5))
	{
		unsigned int currentRenderMode = static_cast<unsigned int>(myRenderMode);
		currentRenderMode--;
		if (currentRenderMode == -1)
		{
			currentRenderMode = static_cast<unsigned char>(RenderMode::COUNT) - 1;
		}

		myRenderMode = static_cast<RenderMode>(currentRenderMode);
	}
#endif // _DEBUG

	ComponentHandler::Update();
	
	const std::vector<std::shared_ptr<ModelInstance>> mdlInstancesToRender = SceneHandler::GetActiveScene()->GetModels();
	RenderStateManager::SetBlendState(RenderStateManager::BlendState::Opaque);
	RenderStateManager::SetDepthStencilState(RenderStateManager::DepthStencilState::ReadWrite);
	myDeferredRenderer.GenerateGBuffer(myCamera, mdlInstancesToRender, Timer::GetDeltaTime(), Timer::GetTotalTime());
	myDeferredRenderer.Render(camera, myDirectionalLight, myEnvironmentLight, Timer::GetDeltaTime(), Timer::GetTotalTime());
	//myForwardRenderer.RenderModels(camera, mdlInstancesToRender, myDirectionalLight, myEnvironmentLight);

	RenderStateManager::SetBlendState(RenderStateManager::BlendState::AlphaBlend);
	RenderStateManager::SetDepthStencilState(RenderStateManager::DepthStencilState::ReadOnly);
	myTextRenderer.Render(camera, SceneHandler::GetActiveScene()->GetTexts());
	RenderStateManager::SetBlendState(RenderStateManager::BlendState::Additive);
	myForwardRenderer.RenderParticles(camera, SceneHandler::GetActiveScene()->GetParticleSystems());
}

void GraphicsEngine::EndFrame()
{
	// F1 - This is where we finish our rendering and tell the framework
	// to present our result to the screen.
	DX11::EndFrame();
}

void GraphicsEngine::SetRenderMode(RenderMode aMode)
{
	myRenderMode = aMode;
}

RenderMode GraphicsEngine::GetRenderMode()
{
	return myRenderMode;
}

RECT GraphicsEngine::GetWindowRectangle()
{
	return windowRect;
}