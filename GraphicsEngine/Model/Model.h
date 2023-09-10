#pragma once
#include <wrl.h>
#include "Material.h"
#include "Skeleton.hpp"
#include "Scene/Transform.h"
#include "Scene/SceneObject.h"

using namespace Microsoft::WRL;

struct ID3D11Buffer;
struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11PixelShader;

class Model
{
	public:
		struct MeshData
		{
			ComPtr<ID3D11Buffer> myVertexBuffer;
			ComPtr<ID3D11Buffer> myIndexBuffer;

			UINT myPrimitiveTopology;
			UINT myNumberOfVertices;
			UINT myNumberOfIndices;
			UINT myStride;
			UINT myOffset;

			ComPtr<ID3D11InputLayout> myInputLayout;
			ComPtr<ID3D11VertexShader> myVertexShader;
			ComPtr<ID3D11PixelShader> myPixelShader;
			std::shared_ptr<Material> myMaterial;
		};

		void Init(MeshData& someMeshData, const std::string& aPath);
		void Init(std::vector<MeshData>& someMeshData, const std::string& aPath);
		void Init(const std::vector<Model::MeshData>& someMeshData, const std::string& aPath, const Skeleton& aSkeleton);

		void SetName(const std::string& aName);

		FORCEINLINE MeshData& GetMeshData(int aIndex) { return myMeshData[aIndex]; }
		FORCEINLINE std::string const& GetName() const { return myName; }
		FORCEINLINE std::string const& GetPath() const { return myPath; }
		FORCEINLINE size_t  GetNumMeshes() const { return myMeshData.size(); }
		FORCEINLINE Skeleton* GetSkeleton() { return &mySkeleton; }
		void AddAnimation(Animation& anAnimation);

		FORCEINLINE void AddAnimName(const std::string& aName) { myAnimationNames.push_back(aName); }
		FORCEINLINE std::vector<std::string> const& GetAnimNames() const { return myAnimationNames; }
		FORCEINLINE void RemoveAnimName(const std::string& aName) {
			for (size_t i = 0; i < myAnimationNames.size(); i++)
			{
				if (myAnimationNames[i] == aName)
					myAnimationNames.erase(myAnimationNames.begin() + i);
			}
		}

	private:
		std::vector<MeshData> myMeshData;
		std::vector<std::string> myAnimationNames;
		std::string myPath;
		std::string myName;
		Skeleton mySkeleton;
};