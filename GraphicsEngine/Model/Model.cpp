#include "GraphicsEngine.pch.h"
#include "Model.h"
#include <filesystem>

void Model::Init(MeshData& someMeshData, const std::string& aPath)
{
	myMeshData.push_back(someMeshData);
	myPath = aPath;
	myName = aPath;
}

void Model::Init(std::vector<MeshData>& someMeshData, const std::string& aPath)
{
	myMeshData = someMeshData;
	myPath = aPath;
	myName = aPath;
}

void Model::Init(const std::vector<Model::MeshData>& someMeshData, const std::string& aPath,
	const Skeleton& aSkeleton)
{
	myMeshData = someMeshData;
	myPath = aPath;
	myName = aPath;
	mySkeleton = aSkeleton;
}

void Model::SetName(const std::string& aName)
{
	myName = aName;
}

void Model::AddAnimation(Animation& anAnimation)
{
	mySkeleton.myAnimations.insert({ anAnimation.myName, anAnimation });
}