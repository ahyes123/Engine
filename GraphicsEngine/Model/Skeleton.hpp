#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "Animation.hpp"
#include "Matrix4x4.hpp"

struct Skeleton
{
	std::string myName;

	struct Bone
	{
		Matrix4x4f BindPoseInverse;
		int Parent;
		std::vector<unsigned int> Children;
		std::string Name;
	};

	std::vector<Bone> myBones;
	std::unordered_map<std::string, size_t> myBoneNameToIndex;
	std::unordered_map<std::string, Animation> myAnimations;

	const Bone* GetRoot() const { if (!myBones.empty()) return &myBones[0]; return nullptr; }
};

