#include "GraphicsEngine.pch.h"
#include "InputHandler.h"
#include <iostream>

namespace CommonUtilities
{
	bool InputHandler::myMouseTwoIsDown;
	bool InputHandler::myPrevMouseTwoState;
	bool InputHandler::myMouseOneIsDown;
	bool InputHandler::myPrevMouseOneState;
	bool InputHandler::myCaptured;

	std::bitset<256> InputHandler::myCurrentFrame;
	std::bitset<256> InputHandler::myPrevFrame;

	Vector2f InputHandler::myCurrentMousePosition;
	Vector2f InputHandler::myPrevMousePosition;

	int InputHandler::myScrollDirection;
	HWND InputHandler::myOwnerHWND;

	InputHandler::InputHandler()
	{
		const Vector2f zeroPoint = { 0, 0 };
		myCurrentMousePosition = zeroPoint;
		myPrevMousePosition = zeroPoint;
		myScrollDirection = 0;
		myMouseTwoIsDown = false;
		myPrevMouseTwoState = false;
		myMouseOneIsDown = false;
		myPrevMouseOneState = false;
		myCaptured = false;
	}

	void InputHandler::Init(HWND aWindowHandle)
	{
		myOwnerHWND = aWindowHandle;
	}

	bool InputHandler::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			case WM_KEYDOWN:
				GetKeyDown(wParam);
				return true;
				
			case WM_KEYUP:
				GetKeyUp(wParam);
				return true;

			case WM_LBUTTONUP:
				myMouseOneIsDown = false;
				return true;

			case WM_LBUTTONDOWN:
				myMouseOneIsDown = true;
				return true;

			case WM_RBUTTONDOWN:
				myMouseTwoIsDown = true;
				return true;

			case WM_RBUTTONUP:
				myMouseTwoIsDown = false;
				return true;
				
			case WM_MOUSEWHEEL:
				myScrollDirection = GET_WHEEL_DELTA_WPARAM(wParam);
				return true;

			case WM_MOUSEMOVE:
				POINT point;
				if (GetCursorPos(&point))
				{
					myCurrentMousePosition = { (float)point.x, (float)point.y };
				}
				return true;

			default:
				break;
		}
		return false;
	}

	void InputHandler::Update()
	{
		myPrevFrame = myCurrentFrame;

		if (!myCaptured)
			myPrevMousePosition = myCurrentMousePosition;

		myPrevMouseOneState = myMouseOneIsDown;
		myPrevMouseTwoState = myMouseTwoIsDown;
		myScrollDirection = 0;
	}

	bool InputHandler::GetKeyIsPressed(WPARAM wParam)
	{
		if (myCurrentFrame.test(wParam) == true && myPrevFrame.test(wParam) == false)
		{
			return true;
		}
		return false;
	}

	bool InputHandler::GetKeyIsHeld(WPARAM wParam)
	{
		return myCurrentFrame.test(wParam);
	}

	bool InputHandler::GetKeyWasReleased(WPARAM wParam)
	{
		if (myCurrentFrame.test(wParam) == false && myPrevFrame.test(wParam) == true)
		{
			return true;
		}
		return false;
	}

	Vector2f InputHandler::GetMousePosition()
	{
		return myCurrentMousePosition;
	}

	Vector2f InputHandler::GetMousePositionDelta()
	{
		return Vector2f(myCurrentMousePosition.x - myPrevMousePosition.x, myCurrentMousePosition.y - myPrevMousePosition.y);
	}

	bool InputHandler::GetMouseOneWasReleased()
	{
		if (myMouseOneIsDown == false && myPrevMouseOneState == true)
		{
			return true;
		}
		return false;
	}

	bool InputHandler::GetMouseOneIsHeld()
	{
		return myMouseOneIsDown;
	}

	bool InputHandler::GetMouseOneWasPressed()
	{
		if (myMouseOneIsDown == true && myPrevMouseOneState == false)
		{
			return true;
		}
		return false;
	}

	bool InputHandler::GetMouseTwoWasPressed()
	{
		if (myMouseTwoIsDown == true && myPrevMouseTwoState == false)
		{
			return true;
		}
		return false;
	}

	bool InputHandler::GetMouseTwoIsHeld()
	{
		return myMouseTwoIsDown;
	}

	bool InputHandler::GetMouseTwoWasReleased()
	{
		if (myMouseTwoIsDown == false && myPrevMouseTwoState == true)
		{
			return true;
		}
		return false;
	}

	bool InputHandler::GetHasScrolledUp()
	{
		if (myScrollDirection > 0)
		{
			return true;
		}
		return false;
	}

	bool InputHandler::GetHasScrolledDown()
	{
		if (myScrollDirection < 0)
		{
			return true;
		}
		return false;
	}

	void InputHandler::SetMousePosition(const int anX, const int aY)
	{
		SetCursorPos(anX, aY);
		myPrevMousePosition = { (float)anX, (float)aY };
	}

	void InputHandler::CaptureMouse()
	{
		RECT clipRect;

		myCaptured = true;

		GetClientRect(myOwnerHWND, &clipRect);

		POINT upperLeft;
		upperLeft.x = clipRect.left;
		upperLeft.y = clipRect.top;

		POINT lowerRight;
		lowerRight.x = clipRect.right;
		lowerRight.y = clipRect.bottom;

		MapWindowPoints(myOwnerHWND, nullptr, &upperLeft, 1);
		MapWindowPoints(myOwnerHWND, nullptr, &lowerRight, 1);

		clipRect.left = upperLeft.x;
		clipRect.top = upperLeft.y;
		clipRect.right = lowerRight.x;
		clipRect.bottom = lowerRight.y;

		ClipCursor(&clipRect);
	}

	void InputHandler::ReleaseMouse()
	{
		ClipCursor(nullptr);
		myCaptured = false;
	}

	void InputHandler::ShowMouse()
	{
		ShowCursor(true);
	}

	void InputHandler::HideMouse()
	{
		ShowCursor(false);
	}

	void InputHandler::GetKeyDown(WPARAM wParam)
	{
		myCurrentFrame.set(wParam);
	}

	void InputHandler::GetKeyUp(WPARAM wParam)
	{
		myCurrentFrame.reset(wParam);
	}
}