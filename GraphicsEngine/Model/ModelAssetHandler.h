#pragma once
#include <memory>
#include <unordered_map>
#include <string>

#include "Material.h"
#include "ModelInstance.h"

class Model;

class ModelAssetHandler
{
	static std::unordered_map<std::wstring, std::shared_ptr<Model>> myModelRegistry;
	static std::unordered_map<std::wstring, std::shared_ptr<Material>> myMaterialRegistry;


public:
	bool Initialize() const;
	static std::shared_ptr<ModelInstance> CreateCube(const std::wstring& aName);
	static std::shared_ptr<ModelInstance> LoadModel(const std::wstring& someFilePath);
	static std::shared_ptr<ModelInstance> LoadModelWithAnimation(const std::wstring& aModelPath, const std::wstring& aAnimationPath);
	static bool LoadAnimation(const std::wstring& aModelName, const std::wstring& someFilePath);

	std::shared_ptr<Model> GetModel(const std::wstring& someFilePath) const;
	static std::shared_ptr<ModelInstance> GetModelInstance(const std::wstring& someFilePath);

	static HRESULT CreateInputLayout(std::string* aVSData, ComPtr<ID3D11InputLayout>& outInputLayout);
private:
	static int GetID();
};

