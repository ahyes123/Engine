#pragma once
#include "Engine/EC/ECEssentials.hpp"
#include "Vector3.hpp"
#include "Matrix4x4.hpp"
#include "Model.h"

class MeshComponent : public Component
{
public:
	MeshComponent();
	~MeshComponent() override = default;

	void Init() override;
	void Update() override;
	static std::string GetFactoryName() { return "MeshComponent"; }
	static std::shared_ptr<Component> Create() { return std::make_shared<MeshComponent>(); }

	void SetModel(std::shared_ptr<Model> aModel);
	std::shared_ptr<Model> GetModel() { return myModel; }

	FORCEINLINE Model::MeshData& GetMeshData(unsigned int anIndex) { return myModel->GetMeshData(anIndex); }
	FORCEINLINE size_t GetNumMeshes() const { return myModel->GetNumMeshes(); }
	FORCEINLINE std::string const& GetPath() const { return myPath; }
	FORCEINLINE bool HasBones() const { return myModel->GetSkeleton()->GetRoot() != nullptr; }

	Matrix4x4f myBoneTransforms[128];
private:

	std::shared_ptr<Model> myModel;

	std::string myPath;

	void ReloadMesh();
};

REGISTER_COMPONENT(MeshComponent);