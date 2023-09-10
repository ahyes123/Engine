#pragma once
#include <memory>
#include <unordered_map>
#include <string>

#include "Material.h"
#include "ModelInstance.h"

class Model;

class ModelAssetHandler
{
	static std::unordered_map<std::string, std::shared_ptr<Model>> myModelRegistry;
	static std::unordered_map<std::string, std::shared_ptr<Material>> myMaterialRegistry;


public:
	bool Initialize() const;
	static std::shared_ptr<Model> CreateCube(const std::string& aName);
	static std::shared_ptr<Model> LoadModel(const std::string& someFilePath);
	static std::shared_ptr<Model> LoadModelWithAnimation(const std::string& aModelPath, const std::string& aAnimationPath);
	static bool LoadAnimation(const std::string& aModelName, const std::string& someFilePath);

	std::shared_ptr<Model> GetModel(const std::string& someFilePath) const;
	static std::shared_ptr<Model> GetModelInstance(const std::string& someFilePath);

	static HRESULT CreateInputLayout(std::string* aVSData, ComPtr<ID3D11InputLayout>& outInputLayout);

	static void SetModelTexture(std::shared_ptr<Model> aMdl, const std::string& aName);
private:
};

