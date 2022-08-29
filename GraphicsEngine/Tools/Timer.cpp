#include "GraphicsEngine.pch.h"
#include "Timer.h"

namespace CommonUtilities
{
	std::chrono::time_point<std::chrono::high_resolution_clock> Timer::myStartTime;
	std::chrono::duration<float> Timer::myDeltaTime;
	std::chrono::duration<double> Timer::myTotalTime;
	float Timer::myTimeScale;

	void Timer::Init()
	{
		myStartTime = std::chrono::high_resolution_clock::now();
		myDeltaTime = std::chrono::high_resolution_clock::now() - myStartTime;
		myTotalTime = std::chrono::high_resolution_clock::now() - myStartTime;
	}

	void Timer::Update()
	{
		auto oldTime = myTotalTime;
		myTotalTime = std::chrono::high_resolution_clock::now() - myStartTime;
		myDeltaTime = myTotalTime - oldTime;
	}

	float Timer::GetDeltaTime()
	{
		return myDeltaTime.count() * myTimeScale;
	}

	float Timer::GetUnscaledDeltaTime()
	{
		return myDeltaTime.count();
	}

	double Timer::GetTotalTime()
	{
		return myTotalTime.count();
	}
}