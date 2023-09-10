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

	unsigned int myLength = 0;
	float myDuration = 0.f;
	float myFPS = 0.f;
	AnimationState myState = AnimationState::Finished;
	std::string myName = "";
	size_t myCurrentFrame = 0;
};
