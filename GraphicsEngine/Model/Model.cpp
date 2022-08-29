#include "GraphicsEngine.pch.h"
#include "Model.h"
#include <filesystem>

void Model::Init(MeshData& someMeshData, const std::wstring& aPath)
{
	myMeshData.push_back(someMeshData);
	myPath = aPath;
	std::filesystem::path name(aPath);
	myName = name.filename();
}

void Model::Init(std::vector<MeshData>& someMeshData, const std::wstring& aPath)
{
	myMeshData = someMeshData;
	myPath = aPath;
	std::filesystem::path name(aPath);
	myName = name.filename();
}

void Model::Init(const std::vector<Model::MeshData>& someMeshData, const std::wstring& aPath,
	const Skeleton& aSkeleton)
{
	myMeshData = someMeshData;
	myPath = aPath;
	std::filesystem::path name(aPath);
	myName = name.filename();
	mySkeleton = aSkeleton;
}

void Model::SetName(const std::wstring& aName)
{
	myName = aName;
}

void Model::AddAnimation(Animation& anAnimation)
{
	mySkeleton.myAnimations.insert({ anAnimation.myName, anAnimation });
}