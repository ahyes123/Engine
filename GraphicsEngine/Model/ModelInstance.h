#pragma once
#include "Model.h"

class ModelInstance
{
	std::shared_ptr<Model> myModel;
	Animation myCurrentAnimation;
	float myTimer;
	bool isLooping;
	//int myId;
	std::string myName;
	std::string myPath;

	public:
		Matrix4x4f myBoneTransforms[128];

		ModelInstance() = default;
		void Init(std::shared_ptr<Model> aModel);
		void Update(float aDeltaTime);
		void UpdateAnimationHierarchy(size_t aCurrentFrame, unsigned aBoneIDx, const Animation* anAnimation,
									  Matrix4x4f& aParentTransform, Matrix4x4f* outboneTransforms, const float& anAlpha);

		void SetAnimation(const std::string& aName, bool aLoop = true);

	FORCEINLINE Model::MeshData& GetMeshData(unsigned int anIndex) { return myModel->GetMeshData(anIndex); }
	FORCEINLINE size_t GetNumMeshes() const { return myModel->GetNumMeshes(); }
	FORCEINLINE std::string const& GetName() const { return myName; }
	FORCEINLINE std::string const& GetPath() const { return myPath; }
	FORCEINLINE void SetName(const std::string& aName) { myName = aName; }
	FORCEINLINE bool HasBones() const { return myModel->GetSkeleton()->GetRoot() != nullptr; }
	//FORCEINLINE int const& GetId() const { return myId; }
	//FORCEINLINE void SetId(int aId) { myId = aId; }
	FORCEINLINE std::vector<std::string> const& GetAnimNames() const { return myModel->GetAnimNames(); }
	FORCEINLINE void RemoveAnimName(const std::string& aName) { myModel->RemoveAnimName(aName); }
	FORCEINLINE Animation const& GetCurrentAnimation() const { return myCurrentAnimation; }
};