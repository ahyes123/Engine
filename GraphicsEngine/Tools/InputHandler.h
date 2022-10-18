#pragma once
#include <bitset>
#include "Math/Vector2.hpp"

namespace CommonUtilities
{
	class InputHandler
	{
		public:
			InputHandler();
			static void Init(HWND aWindowHandle);
			static bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);
			static void Update();

			//Button Presses
			static bool GetKeyIsHeld(WPARAM wParam);
			static bool GetKeyWasReleased(WPARAM wParam);
			static bool GetKeyIsPressed(WPARAM wParam);
			 
			//Mouse
			static Vector2f GetMousePosition();
			static Vector2f GetMousePositionDelta();
			 
			static bool GetMouseOneWasPressed();
			static bool GetMouseOneIsHeld();
			static bool GetMouseOneWasReleased();
			 
			static bool GetMouseTwoWasPressed();
			static bool GetMouseTwoIsHeld();
			static bool GetMouseTwoWasReleased();
			 
			static bool GetHasScrolledUp();
			static bool GetHasScrolledDown();
			 
			static void SetMousePosition(const int anX, const int aY);

			static void CaptureMouse();
			static void ReleaseMouse();
			static void ShowMouse();
			static void HideMouse();

			static Vector2f myPrevMousePosition;
		private:
			static bool myMouseTwoIsDown;
			static bool myPrevMouseTwoState;
			static bool myMouseOneIsDown;
			static bool myPrevMouseOneState;
			static bool myCaptured;
			static HWND myOwnerHWND;

			static void GetKeyDown(WPARAM wParam);
			static void GetKeyUp(WPARAM wParam);
			 
			static std::bitset<256> myCurrentFrame;
			static std::bitset<256> myPrevFrame;

			static Vector2f myCurrentMousePosition;
			 
			static int myScrollDirection;
	};
}
