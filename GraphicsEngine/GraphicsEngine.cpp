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
#include "Light/PointLight.hpp"
#include "Light/SpotLight.hpp"
#include "imgui/imgui.h"
#include <shellapi.h>
using namespace CommonUtilities;
using std::filesystem::directory_iterator;

RenderMode GraphicsEngine::myRenderMode;
RECT GraphicsEngine::windowRect;
HWND GraphicsEngine::myWindowHandle;
std::shared_ptr<Camera> GraphicsEngine::myCamera;
std::array<FLOAT, 4> GraphicsEngine::ourClearColor;
bool GraphicsEngine::myAutoSave;
bool GraphicsEngine::myClearColorBlending;
bool GraphicsEngine::myFileExists;
float GraphicsEngine::myClearColorBlendFactor;
std::string GraphicsEngine::myCurrentClearColorPreset;
std::array<std::array<FLOAT, 4>, 2> GraphicsEngine::myClearColorPresets;

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
	std::shared_ptr<PointLight> point = LightAssetHandler::CreatePointLight({ 0, 0, 1 }, 10000, 1000, 1, { 50, 50, 0 });
	std::shared_ptr<SpotLight> spot = LightAssetHandler::CreateSpotLight({ 1, 0, 0 }, 50000, 1000, 1, 5, 50, { 0, -1, 0 }, { 0, 100, 0 });

	myLights.push_back(point);
	myLights.push_back(spot);

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

	if (!myShadowRenderer.Initialize())
		return false;

	if (!myTextRenderer.Initialize())
		return false;

	return true;
}

LRESULT CALLBACK GraphicsEngine::WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	// We want to be able to access the Graphics Engine instance from inside this function.
	static bool dropped;
	static GraphicsEngine* graphicsEnginePtr = nullptr;
	if (!dropped)
	{
		DragAcceptFiles(hWnd, true);
		dropped = true;
	}
	if (uMsg == WM_DROPFILES)
	{
		DragDrop(wParam);
		return 0;
	}
	if (uMsg == WM_DESTROY || uMsg == WM_CLOSE)
	{
#ifdef _DEBUG
		if (myAutoSave)
		{
			Editor::SaveScenes();
			Editor::SaveClearColorPreset(myCurrentClearColorPreset);
			Editor::SaveSettings();
		}
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

void GraphicsEngine::DragDrop(WPARAM aWparam)
{
	HDROP hDrop = (HDROP)aWparam;
	TCHAR filePath[MAX_PATH];
	DragQueryFile(hDrop, 0, filePath, MAX_PATH);
	std::wstring wpath = filePath;
	std::filesystem::path path = std::filesystem::path(wpath.begin(), wpath.end());
	if (path.extension() == ".fbx")
	{
		Editor::EditorActions action;
		entt::entity ent = SceneHandler::GetActiveScene()->GetRegistry().create();
		std::shared_ptr<ModelInstance> mdl = ModelAssetHandler::LoadModel(path);
		SceneHandler::GetActiveScene()->AddModelInstance(mdl, ent);
		action.AddedObject = true;
		action.Object = mdl;
		action.oldEntity = ent;
		Editor::AddEditorAction(action);
	}
	else if (path.extension() == ".json")
	{
		SceneHandler::LoadScene(path);
	}
	else if (path.extension() == ".dds")
	{
		const std::string animPath = "./Models/Textures";
		bool found = false;
		for (const auto& file : directory_iterator(animPath))
		{
			if (path == file.path())
			{
				found = true;
				break;
			}
		}
		if (found)
		{

		}
		else
		{
			const std::string animPath = "./Models/Textures";
			static int numCount = 1;
			std::filesystem::path originalPath = path;
			for (const auto& file : directory_iterator(animPath))
			{
				std::string fileName = file.path().string();
				std::filesystem::path test(file);
				if (path.filename() == test.filename())
				{
					path = path.replace_extension("");
					path += "_";
					path += std::to_string(numCount);
					path += ".dds";
					numCount++;
				}
			}
			BOOL f = true;
			std::filesystem::path newFileName = L"./Models/Textures/" + originalPath.filename().wstring();
			CopyFile(originalPath.wstring().c_str(), newFileName.c_str(), f);
			EditorInterface::SetTexture(newFileName.filename().wstring());
		}
	}
	DragFinish(hDrop);
}

void GraphicsEngine::BeginFrame()
{
	// F1 - This is where we clear our buffers and start the DX frame.

	DX11::BeginFrame(ourClearColor);
	RenderStateManager::ResetStates();
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

	//ImGui::Begin("Lights");
	//ImGui::DragFloat("Range", &myLights[0]->myLightBufferData.Range, 1, 0, INT_MAX);
	//ImGui::DragFloat("Intensity", &myLights[0]->myLightBufferData.Intensity, 1, 0, INT_MAX);
	//ImGui::DragFloat3("Position", &myLights[0]->myLightBufferData.Position.x, 1, -INT_MAX, INT_MAX);
	//ImGui::DragFloat3("Color", &myLights[0]->myLightBufferData.Color.x, 0.01f, 0, 1);
	//ImGui::DragFloat("Inner rad", &myLights[0]->myLightBufferData.SpotInnerRadius, 1, 0, INT_MAX);
	//ImGui::DragFloat("Outer rad", &myLights[0]->myLightBufferData.SpotOuterRadius, 1, 0, INT_MAX);
	//ImGui::DragFloat3("Direction", &myLights[0]->myLightBufferData.Direction.x, 0.01f, -1, 1);
	//ImGui::End();

	const std::vector<std::shared_ptr<ModelInstance>> mdlInstancesToRender = SceneHandler::GetActiveScene()->GetModels();
	myDirectionalLight->ClearShadowMap();
	myDirectionalLight->SetShadowMapAsDepth();
	myShadowRenderer.Render(myDirectionalLight, mdlInstancesToRender);
	RenderStateManager::SetBlendState(RenderStateManager::BlendState::Opaque);
	RenderStateManager::SetDepthStencilState(RenderStateManager::DepthStencilState::ReadWrite);
	myDeferredRenderer.GenerateGBuffer(myCamera, mdlInstancesToRender, Timer::GetDeltaTime(), Timer::GetTotalTime());
	myDeferredRenderer.Render(camera, myDirectionalLight, myLights, myEnvironmentLight, Timer::GetDeltaTime(), Timer::GetTotalTime());
	//myForwardRenderer.RenderModels(camera, mdlInstancesToRender, myDirectionalLight, myEnvironmentLight);

	RenderStateManager::SetBlendState(RenderStateManager::BlendState::TextBlend);
	RenderStateManager::SetDepthStencilState(RenderStateManager::DepthStencilState::ReadOnly);
	myTextRenderer.Render(camera, SceneHandler::GetActiveScene()->GetTexts());

	RenderStateManager::SetBlendState(RenderStateManager::BlendState::Additive);
	myForwardRenderer.RenderParticles(camera, SceneHandler::GetActiveScene()->GetParticleSystems());
	DX11::Context->OMSetRenderTargets(1, DX11::BackBuffer.GetAddressOf(), DX11::DepthBuffer.Get());
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