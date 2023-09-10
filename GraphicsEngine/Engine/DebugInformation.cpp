#include "GraphicsEngine.pch.h"
#include "DebugInformation.h"
#include "../Text/Text.h"
#include "../Text/TextFactory.h"
#include "../GraphicsEngine.h"
#include "../Tools/Timer.h"
#include "../Scene/SceneHandler.h"

std::shared_ptr<Text> DebugInformation::myFpsText;
unsigned short DebugInformation::myRealFPS;
unsigned long DebugInformation::myRealFPSAvarage;

void DebugInformation::ShowFPSText()
{
	if (myFpsText.get() == nullptr)
	{
		myFpsText = TextFactory::CreateText(L"FPS: ", 1, 12, true);
		myFpsText->SetPosition(-1, 0.85f, 0);
	}
	std::wstring fpsText;

	fpsText.append(L"FPS: ");

	myRealFPS = static_cast<unsigned short>(1.0f / CommonUtilities::Timer::GetUnscaledDeltaTime());

	static float timeInter = 0;
	static int iterations = 0;
	iterations++;
	timeInter += CommonUtilities::Timer::GetUnscaledDeltaTime();
	myRealFPSAvarage += myRealFPS;
	static int avarageFPS = 0;
	if (timeInter >= 0.3f)
	{
		timeInter = 0;
		avarageFPS = myRealFPSAvarage / iterations;
		myRealFPSAvarage = 0;
		iterations = 0;
	}
	fpsText.append(std::to_wstring(avarageFPS));
	myFpsText->SetText(fpsText);
}
