#pragma once
#include <string>
#include <vector>
#include "Transform.h"

struct Animation
{
	enum class AnimationState
	{
		Playing,
		Finished
	};

	struct Frame
	{
		std::vector<Transform> LocalTransform;
	};

	std::vector<Frame> myFrames;

	unsigned int myLength;
	float myDuration;
	float myFPS;
	AnimationState myState;
	std::wstring myName;
	size_t myCurrentFrame = 0;
};
