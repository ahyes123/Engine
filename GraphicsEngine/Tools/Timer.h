#pragma once
#include <chrono>

namespace CommonUtilities
{
	class Timer
	{
		public:
			Timer(const Timer& aTimer) = delete;
			Timer& operator=(const Timer& aTimer) = delete;

			static void Init();
			static void Update();
			static float GetDeltaTime();
			static float GetUnscaledDeltaTime();
			static double GetTotalTime();
			static float const GetTimeScale() { return myTimeScale; } const
			static void SetTimeScale(const float& aTimeScale) { myTimeScale = aTimeScale; }

		private:
			static std::chrono::time_point<std::chrono::high_resolution_clock> myStartTime;
			static std::chrono::duration<float> myDeltaTime;
			static std::chrono::duration<double> myTotalTime;
			static float myTimeScale;
	};
}

