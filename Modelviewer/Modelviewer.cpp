// Exclude things we don't need from the Windows headers
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "Modelviewer.h"
#include "GraphicsEngine.h"
#include "Tools/Timer.h"
#include "Tools/InputHandler.h"
#include "../External/imgui/imgui.h"
#include "../External/imgui/ImGuizmo.h"
#include "../External/imgui/imgui_impl_dx11.h"
#include "../External/imgui/imgui_impl_win32.h"
#include "../GraphicsEngine/Engine/DX11.h"
#include "../GraphicsEngine/Engine/DebugInformation.h"

using namespace CommonUtilities;
#ifdef _DEBUG
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // _DEBUG

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);
#ifdef _DEBUG
	FILE* consoleOut;
	AllocConsole();
	freopen_s(&consoleOut, "CONOUT$", "w", stdout);
	setvbuf(consoleOut, nullptr, _IONBF, 1024);
#endif

	MSG msg = { 0 };

	GraphicsEngine graphicsEngine;

	RECT desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);

	const SIZE windowSize = { desktop.right, desktop.bottom }; // ÄNDRA DETTA FÖR ATT IMGUI SKA FUNKA

	bool bShouldRun = graphicsEngine.Initialize(
		(GetSystemMetrics(SM_CXSCREEN) - windowSize.cx) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - windowSize.cy) / 2,
		windowSize.cx,
		windowSize.cy,
		true
	);

	Timer::Init();
	InputHandler::Init(graphicsEngine.GetWindowHandle());

#ifdef _DEBUG
	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(graphicsEngine.GetWindowHandle());
	ImGui_ImplDX11_Init(DX11::Device.Get(), DX11::Context.Get());
	ImGui::StyleColorsDark();
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 18.f);
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif // _DEBUG

	while (bShouldRun)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
#ifdef _DEBUG
			ImGui_ImplWin32_WndProcHandler(graphicsEngine.GetWindowHandle(), msg.message, msg.wParam, msg.lParam);
#endif // _DEBUG
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			// Should update input here.

			InputHandler::UpdateEvents(msg.message, msg.wParam, msg.lParam);

			if (msg.message == WM_QUIT)
			{
				bShouldRun = false;
			}
		}

		// REMEMBER!
		// The frame update for the game does NOT happen inside the PeekMessage loop.
		// This would cause the game to only update if there are messages and also run
		// the update several times per frame (once for each message).

		graphicsEngine.BeginFrame();

		Timer::Update();
#ifdef _DEBUG
		DebugInformation::ShowFPSText();
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
#endif // _DEBUG
		graphicsEngine.RenderFrame();
#ifdef _DEBUG
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // _DEBUG

		graphicsEngine.EndFrame();

		InputHandler::Update();
	}

	return 0;
}
