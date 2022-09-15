#include "GraphicsEngine.pch.h"
#include "ModelAssetHandler.h"

#include <algorithm>

#include "Model.h"
#include <fstream>
#include <d3d11.h>
#include "Engine/DX11.h"
#include "FBXImporter.h"
#include "Vertex.hpp"
#include "Texture/TextureAssetHandler.h"
#include "UtilityFunctions.hpp"
#include "../Scene/SceneHandler.h"
#include <filesystem>

std::unordered_map<std::wstring, std::shared_ptr<Model>> ModelAssetHandler::myModelRegistry;
std::unordered_map<std::wstring, std::shared_ptr<Material>> ModelAssetHandler::myMaterialRegistry;

std::shared_ptr<ModelInstance> ModelAssetHandler::CreateCube(const std::wstring& aName)
{
	std::vector<Vertex> mdlVertices = {
		// Front Face
	{
		-50.f, -50.f, -50.f, 1,
		0, 1, 0, 1
	},
	{
		-50.f, 50.f, -50.f, 1,
		0, 0, 1, 1
	},
	{
		50.f, 50.f, -50.f, 1,
		1, 0, 1, 1
	},
	{
		50.f, -50.f, -50.f, 1,
		1, 1, 1, 1
	},
		// Back Face
	{
		-50.f, -50.f, 50.f, 1,
		1, 1, 1, 1
	},
	{
		50.f, -50.f, 50.f, 1,
		0, 1, 1, 1
	},
	{
		50.f, 50.f, 50.f, 1,
		0, 0, 1, 1
	},
	{
		-50.f, 50.f, 50.f, 1,
		1, 0, 1, 1
	},
		// Top Face
	{
		-50.f, 50.f, -50.f, 1,
		0, 1, 1, 1
	},
	{
		-50.f, 50.f, 50.f, 1,
		0, 0, 1, 1
	},
	{
		50.f, 50.f, 50.f, 1,
		1, 0, 1, 1
	},
	{
		50.f, 50.f, -50.f, 1,
		1, 1, 1, 1
	},
		// Bottom Face
	{
		-50.f, -50.f, -50.f, 1,
		1, 1, 1, 1
	},
	{
		50.f, -50.f, -50.f, 1,
		0, 1, 1, 1
	},
	{
		50.f, -50.f, 50.f, 1,
		0, 0, 1, 1
	},
	{
		-50.f, -50.f, 50.f, 1,
		1, 0, 1, 1
	},
		// Left Face
	{
		-50.f, -50.f, 50.f, 1,
		0, 1, 1, 1
	},
	{
		-50.f, 50.f, 50.f, 1,
		0, 0, 1, 1
	},
	{
		-50.f, 50.f, -50.f, 1,
		1, 0, 1, 1
	},
	{
		-50.f, -50.f, -50.f, 1,
		1, 1, 1, 1
	},
		// Right Face
	{
		50.f, -50.f, -50.f, 1,
		0, 1, 1, 1
	},
	{
		50.f, 50.f, -50.f, 1,
		0, 0, 1, 1
	},
	{
		50.f, 50.f, 50.f, 1,
		1, 0, 1, 1
	},
	{
		50.f, -50.f, 50.f, 1,
		1, 1, 1, 1
	}
	};

	std::vector<unsigned int> mdlIndices = {
		// Front Face
		0, 1, 2,
		0, 2, 3,

		// Back Face
		4, 5, 6,
		4, 6, 7,

		// Top Face
		8, 9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};

	//UV
	{
		mdlVertices[0].UVs[0] = { 0, 1 };
		mdlVertices[1].UVs[0] = { 0, 0 };
		mdlVertices[2].UVs[0] = { 1, 0 };
		mdlVertices[3].UVs[0] = { 1, 1 };

		mdlVertices[4].UVs[0] = { 1, 1 };
		mdlVertices[5].UVs[0] = { 0, 1 };
		mdlVertices[6].UVs[0] = { 0, 0 };
		mdlVertices[7].UVs[0] = { 1, 0 };

		mdlVertices[8].UVs[0] = { 0, 1 };
		mdlVertices[9].UVs[0] = { 0, 0 };
		mdlVertices[10].UVs[0] = { 1, 0 };
		mdlVertices[11].UVs[0] = { 1, 1 };

		mdlVertices[12].UVs[0] = { 0, 0 };
		mdlVertices[13].UVs[0] = { 1, 0 };
		mdlVertices[14].UVs[0] = { 1, 1 };
		mdlVertices[15].UVs[0] = { 0, 1 };

		mdlVertices[16].UVs[0] = { 0, 1 };
		mdlVertices[17].UVs[0] = { 0, 0 };
		mdlVertices[18].UVs[0] = { 1, 0 };
		mdlVertices[19].UVs[0] = { 1, 1 };

		mdlVertices[20].UVs[0] = { 0, 1 };
		mdlVertices[21].UVs[0] = { 0, 0 };
		mdlVertices[22].UVs[0] = { 1, 0 };
		mdlVertices[23].UVs[0] = { 1, 1 };
	}

	//Normals
	{
		for (int i = 0; i < 24; i++)
		{
			if (i < 4)
			{
				mdlVertices[i].Tangent = { 1, 0, 0 };
				mdlVertices[i].Binormal = { 0, -1, 0 };
				mdlVertices[i].Normal = { 0, 0, -1 };
			}
			else if (i < 8)
			{
				mdlVertices[i].Tangent = { -1, 0, 0 };
				mdlVertices[i].Binormal = { 0, -1, 0 };
				mdlVertices[i].Normal = { 0, 0, 1 };
			}
			else if (i < 12)
			{
				mdlVertices[i].Tangent = { 1, 0, 0 };
				mdlVertices[i].Binormal = { 0, 0, -1 };
				mdlVertices[i].Normal = { 0, 1, 0 };
			}
			else if (i < 16)
			{
				mdlVertices[i].Tangent = { 1, 0, 0 };
				mdlVertices[i].Binormal = { 0, 0, 1 };
				mdlVertices[i].Normal = { 0, -1, 0 };
			}
			else if (i < 20)
			{
				mdlVertices[i].Tangent = { 0, 0, -1 };
				mdlVertices[i].Binormal = { 0, -1, 0 };
				mdlVertices[i].Normal = { -1, 0, 0 };
			}
			else
			{
				mdlVertices[i].Tangent = { 0, 0, 1 };
				mdlVertices[i].Binormal = { 0, -1, 0 };
				mdlVertices[i].Normal = { 1, 0, 0 };
			}
		}
	}

	HRESULT result;

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(mdlVertices.size()) * static_cast<UINT>(sizeof(Vertex));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	vertexSubresourceData.pSysMem = &mdlVertices[0];

	ID3D11Buffer* vertexBuffer;
	result = DX11::Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &vertexBuffer);

	if (FAILED(result))
	{
		return nullptr;
	}


	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = static_cast<UINT>(mdlIndices.size()) * static_cast<UINT>(sizeof(unsigned int));
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData{};
	indexSubresourceData.pSysMem = &mdlIndices[0];

	ID3D11Buffer* indexBuffer;
	result = DX11::Device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &indexBuffer);

	if (FAILED(result))
	{
		return nullptr;
	}

	std::ifstream vsFile;
	vsFile.open("Shaders/DefaultVS.cso", std::ios::binary);
	std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	ID3D11VertexShader* vertexShader;
	result = DX11::Device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
	vsFile.close();

	if (FAILED(result))
	{
		return nullptr;
	}

	std::ifstream psFile;
	psFile.open("Shaders/DefaultPS.cso", std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	ID3D11PixelShader* pixelShader;
	result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
	psFile.close();

	if (FAILED(result))
	{
		return nullptr;
	}

	ComPtr<ID3D11InputLayout> inputLayout;
	result = CreateInputLayout(&vsData, inputLayout);

	if (FAILED(result))
	{
		return nullptr;
	}

	Model::MeshData modelData = {};

	std::shared_ptr<Material> meshMaterial = std::make_shared<Material>();

	if (TextureAssetHandler::LoadTexture(L"Models/Textures/T_Default_C.dds"))
	{
		meshMaterial->SetAlbedoTexture(TextureAssetHandler::GetTexture(L"Models/Textures/T_Default_C.dds"));
	}
	if (TextureAssetHandler::LoadTexture(L"Models/Textures/T_Default_N.dds"))
	{
		meshMaterial->SetNormalTexture(TextureAssetHandler::GetTexture(L"Models/Textures/T_Default_N.dds"));
	}
	if (TextureAssetHandler::LoadTexture(L"Models/Textures/T_Default_M.dds"))
	{
		meshMaterial->SetMaterialTexture(TextureAssetHandler::GetTexture(L"Models/Textures/T_Default_M.dds"));
	}

	modelData.myMaterial = meshMaterial;
	modelData.myNumberOfVertices = static_cast<unsigned int>(mdlVertices.size());
	modelData.myNumberOfIndices = static_cast<unsigned int>(mdlIndices.size());
	modelData.myStride = sizeof(Vertex);
	modelData.myOffset = 0;
	modelData.myVertexBuffer = vertexBuffer;
	modelData.myIndexBuffer = indexBuffer;
	modelData.myVertexShader = vertexShader;
	modelData.myPixelShader = pixelShader;
	modelData.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	modelData.myInputLayout = inputLayout;

	std::shared_ptr<Model> mdl = std::make_shared<Model>();
	mdl->Init(modelData, aName);
	myModelRegistry.insert({ aName, mdl });

	std::shared_ptr<ModelInstance> mdlInstance;
	mdlInstance = std::make_shared<ModelInstance>();
	mdlInstance->Init(myModelRegistry[aName]);
	mdlInstance->SetId(SceneHandler::GetActiveScene()->GetNextId());
	return mdlInstance;
}

std::shared_ptr<ModelInstance> ModelAssetHandler::LoadModel(const std::wstring& someFilePath)
{
	const std::string ansiFileName = std::string(someFilePath.begin(), someFilePath.end());

	TGA::FBXModel tgaModel;

	if (TGA::FBXImporter::LoadModel(ansiFileName, tgaModel))
	{
		std::vector<Model::MeshData> mdlMeshData;
		mdlMeshData.resize(tgaModel.Meshes.size());
		Skeleton mdlSkeleton;
		const bool hasSkeleton = tgaModel.Skeleton.GetRoot();

		if (hasSkeleton)
		{
			for (size_t amount = 0; amount < tgaModel.Skeleton.Bones.size(); amount++)
			{
				Skeleton::Bone bone;

				bone.Children = tgaModel.Skeleton.Bones[amount].Children;
				bone.Name = tgaModel.Skeleton.Bones[amount].Name;
				bone.BindPoseInverse = tgaModel.Skeleton.Bones[amount].BindPoseInverse;
				bone.Parent = tgaModel.Skeleton.Bones[amount].Parent;

				mdlSkeleton.myBones.push_back(bone);
			}
		}

		for (size_t i = 0; i < tgaModel.Meshes.size(); ++i)
		{
			std::vector<Vertex> mdlVertices;

			TGA::FBXModel::FBXMesh& mesh = tgaModel.Meshes[i];

			std::filesystem::path fileName(someFilePath);
			const std::wstring baseName = fileName.filename().replace_extension("");
			const std::wstring albedoFileName = L"Models/Textures/T_" + baseName + L"_C.dds";
			const std::wstring normalFileName = L"Models/Textures/T_" + baseName + L"_N.dds";
			const std::wstring materialFileName = L"Models/Textures/T_" + baseName + L"_M.dds";

			std::shared_ptr<Material> meshMaterial = std::make_shared<Material>();

			if (TextureAssetHandler::LoadTexture(albedoFileName))
			{
				meshMaterial->SetAlbedoTexture(TextureAssetHandler::GetTexture(albedoFileName));
			}
			else if (TextureAssetHandler::LoadTexture(L"Models/Textures/T_Default_C.dds"))
			{
				meshMaterial->SetAlbedoTexture(TextureAssetHandler::GetTexture(L"Models/Textures/T_Default_C.dds"));
			}

			if (TextureAssetHandler::LoadTexture(normalFileName))
			{
				meshMaterial->SetNormalTexture(TextureAssetHandler::GetTexture(normalFileName));
			}
			else if (TextureAssetHandler::LoadTexture(L"Models/Textures/T_Default_N.dds"))
			{
				meshMaterial->SetNormalTexture(TextureAssetHandler::GetTexture(L"Models/Textures/T_Default_N.dds"));
			}

			if (TextureAssetHandler::LoadTexture(materialFileName))
			{
				meshMaterial->SetMaterialTexture(TextureAssetHandler::GetTexture(materialFileName));
			}
			else if (TextureAssetHandler::LoadTexture(L"Models/Textures/T_Default_M.dds"))
			{
				meshMaterial->SetMaterialTexture(TextureAssetHandler::GetTexture(L"Models/Textures/T_Default_M.dds"));
			}

			mdlMeshData[i].myMaterial = meshMaterial;

			for (size_t v = 0; v < mesh.Vertices.size(); ++v)
			{
				Vertex vertex;

				vertex.Position.x = mesh.Vertices[v].Position[0];
				vertex.Position.y = mesh.Vertices[v].Position[1];
				vertex.Position.z = mesh.Vertices[v].Position[2];
				vertex.Position.w = mesh.Vertices[v].Position[3];

				mdlVertices.push_back(vertex);

				for (int vCol = 0; vCol < 4; vCol++)
				{
					mdlVertices[v].VertexColors[vCol].x = mesh.Vertices[v].VertexColors[vCol][0];
					mdlVertices[v].VertexColors[vCol].y = mesh.Vertices[v].VertexColors[vCol][1];
					mdlVertices[v].VertexColors[vCol].z = mesh.Vertices[v].VertexColors[vCol][2];
					mdlVertices[v].VertexColors[vCol].w = mesh.Vertices[v].VertexColors[vCol][3];
				}

				for (int uvCh = 0; uvCh < 4; uvCh++)
				{
					mdlVertices[v].UVs[uvCh].x = mesh.Vertices[v].UVs[uvCh][0];
					mdlVertices[v].UVs[uvCh].y = mesh.Vertices[v].UVs[uvCh][1];
				}

				mdlVertices[v].myBoneIDs = {
					mesh.Vertices[v].BoneIDs[0],
					mesh.Vertices[v].BoneIDs[1],
					mesh.Vertices[v].BoneIDs[2],
					mesh.Vertices[v].BoneIDs[3]
				};

				mdlVertices[v].myBoneWeights = {
					mesh.Vertices[v].BoneWeights[0],
					mesh.Vertices[v].BoneWeights[1],
					mesh.Vertices[v].BoneWeights[2],
					mesh.Vertices[v].BoneWeights[3]
				};

				mdlVertices[v].VertexColors[0] = {
					static_cast<float>(rand() % 100) / 100.f,
					static_cast<float>(rand() % 100) / 100.f,
					static_cast<float>(rand() % 100) / 100.f,
					1.0f
				};

				mdlVertices[v].Tangent = {
					mesh.Vertices[v].Tangent[0],
					mesh.Vertices[v].Tangent[1],
					mesh.Vertices[v].Tangent[2]
				};

				mdlVertices[v].Binormal = {
					mesh.Vertices[v].Binormal[0],
					mesh.Vertices[v].Binormal[1],
					mesh.Vertices[v].Binormal[2]
				};

				mdlVertices[v].Normal = {
					mesh.Vertices[v].Normal[0],
					mesh.Vertices[v].Normal[1],
					mesh.Vertices[v].Normal[2]
				};
			}

			std::vector<unsigned int> mdlIndices = mesh.Indices;

			HRESULT result;

			D3D11_BUFFER_DESC vertexBufferDesc{};
			vertexBufferDesc.ByteWidth = static_cast<UINT>(mdlVertices.size()) * static_cast<UINT>(sizeof(Vertex));
			vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
			vertexSubresourceData.pSysMem = &mdlVertices[0];

			ID3D11Buffer* vertexBuffer;
			result = DX11::Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &vertexBuffer);

			if (FAILED(result))
			{
				return nullptr;
			}

			D3D11_BUFFER_DESC indexBufferDesc{};
			indexBufferDesc.ByteWidth = static_cast<UINT>(mdlIndices.size()) * static_cast<UINT>(sizeof(unsigned int));
			indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			D3D11_SUBRESOURCE_DATA indexSubresourceData{};
			indexSubresourceData.pSysMem = &mdlIndices[0];

			ID3D11Buffer* indexBuffer;
			result = DX11::Device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &indexBuffer);

			if (FAILED(result))
			{
				return nullptr;
			}

			std::ifstream vsFile;
			vsFile.open("Shaders/DefaultVS.cso", std::ios::binary);
			std::string vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
			ID3D11VertexShader* vertexShader;
			result = DX11::Device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertexShader);
			vsFile.close();

			if (FAILED(result))
			{
				return nullptr;
			}

			std::ifstream psFile;
			psFile.open("Shaders/DefaultPS.cso", std::ios::binary);
			std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
			ID3D11PixelShader* pixelShader;
			result = DX11::Device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixelShader);
			psFile.close();

			if (FAILED(result))
			{
				return nullptr;
			}

			ComPtr<ID3D11InputLayout> inputLayout;
			result = CreateInputLayout(&vsData, inputLayout);

			if (FAILED(result))
			{
				return nullptr;
			}

			mdlMeshData[i].myNumberOfVertices = static_cast<unsigned int>(mdlVertices.size());
			mdlMeshData[i].myNumberOfIndices = static_cast<unsigned int>(mdlIndices.size());
			mdlMeshData[i].myStride = sizeof(Vertex);
			mdlMeshData[i].myOffset = 0;
			mdlMeshData[i].myVertexBuffer = vertexBuffer;
			mdlMeshData[i].myIndexBuffer = indexBuffer;
			mdlMeshData[i].myVertexShader = vertexShader;
			mdlMeshData[i].myPixelShader = pixelShader;
			mdlMeshData[i].myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			mdlMeshData[i].myInputLayout = inputLayout;
		}
		std::shared_ptr<Model> mdl = std::make_shared<Model>();

		if (hasSkeleton)
		{
			mdl->Init(mdlMeshData, someFilePath, mdlSkeleton);
		}
		else
		{
			mdl->Init(mdlMeshData, someFilePath);
		}
		myModelRegistry.insert({ someFilePath, mdl });

		std::shared_ptr<ModelInstance> mdlInstance;
		mdlInstance = std::make_shared<ModelInstance>();
		mdlInstance->Init(myModelRegistry[someFilePath]);
		mdlInstance->SetId(SceneHandler::GetActiveScene()->GetNextId());
		return mdlInstance;
	}
	return nullptr;
}

std::shared_ptr<ModelInstance> ModelAssetHandler::LoadModelWithAnimation(const std::wstring& aModelPath, const std::wstring& aAnimationPath)
{
	LoadModel(aModelPath);
	LoadAnimation(aModelPath, aAnimationPath);
	std::shared_ptr<ModelInstance> modelInstance = GetModelInstance(aModelPath);
	return modelInstance;
}

bool ModelAssetHandler::LoadAnimation(const std::wstring& aModelName, const std::wstring& someFilePath)
{
	std::string fileName = std::string(someFilePath.begin(), someFilePath.end());
	size_t index = fileName.find("Animations\\");
	fileName = fileName.substr(index, fileName.size());
	std::string ansiFileName = "./";
	ansiFileName += fileName;


	TGA::FBXAnimation tgaAnimation;

	std::shared_ptr<Model> model = myModelRegistry[aModelName];
	std::vector<std::string> myBoneNames;

	for (auto& bones : model->GetSkeleton()->myBones)
	{
		myBoneNames.push_back(bones.Name);
	}

	if (TGA::FBXImporter::LoadAnimation(ansiFileName, myBoneNames, tgaAnimation))
	{
		Animation result;
		result.myName = std::wstring(tgaAnimation.Name.begin(), tgaAnimation.Name.end());
		result.myDuration = tgaAnimation.Duration;
		result.myFPS = tgaAnimation.FramesPerSecond;
		result.myLength = tgaAnimation.Length;

		for (auto& frame : tgaAnimation.Frames)
		{
			Animation::Frame animFrame;
			for (int amount = 0; amount < frame.LocalTransforms.size(); amount++)
			{
				Matrix4x4f matrix;
				matrix = frame.LocalTransforms[amount];
				Transform transform(matrix);
				transform.ComposeMatrix(true);
				animFrame.LocalTransform.push_back(transform);
			}
			result.myFrames.push_back(animFrame);
		}

		std::wstring name = std::wstring(std::wstring(ansiFileName.begin(), ansiFileName.end()));
		model->GetSkeleton()->myName = name;
		model->AddAnimation(result);
		for (size_t i = 0; i < model->GetAnimNames().size(); i++)
		{
			if (model->GetAnimNames()[i] == name)
			{
				return true;
			}
		}
		model->AddAnimName(name);
		return true;
	}
	return false;
}

bool ModelAssetHandler::Initialize() const
{
	return true;
}

std::shared_ptr<Model> ModelAssetHandler::GetModel(const std::wstring& someFilePath) const
{
	return myModelRegistry[someFilePath];
}

std::shared_ptr<ModelInstance> ModelAssetHandler::GetModelInstance(const std::wstring& someFilePath)
{
	std::shared_ptr<ModelInstance> mdlInstance;
	mdlInstance = std::make_shared<ModelInstance>();
	mdlInstance->Init(myModelRegistry[someFilePath]);
	mdlInstance->SetId(SceneHandler::GetActiveScene()->GetNextId());
	return mdlInstance;
}

HRESULT ModelAssetHandler::CreateInputLayout(std::string* aVSData, ComPtr<ID3D11InputLayout>& outInputLayout)
{
	static D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 3, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	return DX11::Device->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), aVSData->data(), aVSData->size(), outInputLayout.GetAddressOf());
}

void ModelAssetHandler::SetModelTexture(std::shared_ptr<ModelInstance> aMdl, const std::wstring& aName)
{
	for (size_t i = 0; i < aMdl->GetNumMeshes(); ++i)
	{
		std::filesystem::path fileName(aName);
		std::wstring baseName = fileName.filename().replace_extension("");
		std::wstring nameWithoutNumbers = baseName;
		std::wstring undefiledName = baseName;

		baseName = baseName.substr(2, baseName.size());
		size_t index = baseName.find(L"_C");
		if (index > baseName.size())
			index = baseName.find(L"_N");
		if (index > baseName.size())
			index = baseName.find(L"_M");

		nameWithoutNumbers = nameWithoutNumbers.substr(0, index + 4);

		std::shared_ptr<Material> meshMaterial = aMdl->GetMeshData(i).myMaterial;
		if (nameWithoutNumbers.at(nameWithoutNumbers.size() - 1) == L'C')
		{
			undefiledName = L"Models/Textures/" + undefiledName + L".dds";
			if (TextureAssetHandler::LoadTexture(undefiledName))
			{
				meshMaterial->SetAlbedoTexture(TextureAssetHandler::GetTexture(undefiledName));
			}
			else if (TextureAssetHandler::LoadTexture(L"Models/Textures/T_Default_C.dds"))
			{
				meshMaterial->SetAlbedoTexture(TextureAssetHandler::GetTexture(L"Models/Textures/T_Default_C.dds"));
			}
		}

		if (nameWithoutNumbers.at(nameWithoutNumbers.size() - 1) == L'N')
		{
			undefiledName = L"Models/Textures/" + baseName + L".dds";
			if (TextureAssetHandler::LoadTexture(undefiledName))
			{
				meshMaterial->SetNormalTexture(TextureAssetHandler::GetTexture(undefiledName));
			}
			else if (TextureAssetHandler::LoadTexture(L"Models/Textures/T_Default_N.dds"))
			{
				meshMaterial->SetNormalTexture(TextureAssetHandler::GetTexture(L"Models/Textures/T_Default_N.dds"));
			}
		}

		if (nameWithoutNumbers.at(nameWithoutNumbers.size() - 1) == L'M')
		{
			undefiledName = L"Models/Textures/" + undefiledName + L".dds";
			if (TextureAssetHandler::LoadTexture(undefiledName))
			{
				meshMaterial->SetMaterialTexture(TextureAssetHandler::GetTexture(undefiledName));
			}
			else if (TextureAssetHandler::LoadTexture(L"Models/Textures/T_Default_M.dds"))
			{
				meshMaterial->SetMaterialTexture(TextureAssetHandler::GetTexture(L"Models/Textures/T_Default_M.dds"));
			}
		}
	}
}
