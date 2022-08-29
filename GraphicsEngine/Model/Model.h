#pragma once
#include <memory>
#include <string>
#include <vector>
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

class Model : public SceneObject
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

		void Init(MeshData& someMeshData, const std::wstring& aPath);
		void Init(std::vector<MeshData>& someMeshData, const std::wstring& aPath);
		void Init(const std::vector<Model::MeshData>& someMeshData, const std::wstring& aPath, const Skeleton& aSkeleton);

		void SetName(const std::wstring& aName);

		FORCEINLINE MeshData const& GetMeshData(int aIndex) const { return myMeshData[aIndex]; }
		FORCEINLINE std::wstring const& GetName() const { return myName; }
		FORCEINLINE std::wstring const& GetPath() const { return myPath; }
		FORCEINLINE size_t  GetNumMeshes() const { return myMeshData.size(); }
		FORCEINLINE Skeleton* GetSkeleton() { return &mySkeleton; }
		void AddAnimation(Animation& anAnimation);

		FORCEINLINE void AddAnimName(const std::wstring& aName) { myAnimationNames.push_back(aName); }
		FORCEINLINE std::vector<std::wstring> const& GetAnimNames() const { return myAnimationNames; }
		FORCEINLINE void RemoveAnimName(const std::wstring& aName) {
			for (size_t i = 0; i < myAnimationNames.size(); i++)
			{
				if (myAnimationNames[i] == aName)
					myAnimationNames.erase(myAnimationNames.begin() + i);
			}
		}

	private:
		std::vector<MeshData> myMeshData;
		std::vector<std::wstring> myAnimationNames;
		std::wstring myPath;
		std::wstring myName;
		Skeleton mySkeleton;

		Transform myTransform;
};