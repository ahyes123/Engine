#pragma once
#include <memory>
#include "SceneObject.h"
#include "Font.hpp"

class Text : public SceneObject
{
	public:
		struct TextData
		{
			UINT myNumberOfVertices;
			UINT myNumber0fIndices;
			UINT myStride;
			UINT myOffset;
			ComPtr<ID3D11Buffer> myVertexBuffer;
			ComPtr<ID3D11Buffer> myIndexBuffer;
			D3D_PRIMITIVE_TOPOLOGY myPrimitiveTopology;
		};

		void Init(TextData someTextData, std::wstring someText, std::shared_ptr<Font> aFont, bool aIs2D);

		FORCEINLINE TextData GetTextData() const { return myTextData; }
		FORCEINLINE std::shared_ptr<Font> GetFont() const { return myFont; }
		FORCEINLINE bool GetIs2D() const { return myIs2D; }
		void SetIs2D(const bool& aIs2D) { myIs2D = aIs2D; }
		void SetText(const std::wstring someText);
		std::wstring GetText() { return myText; }
		std::wstring const& GetName() const { return myName; }
		void SetName(const std::wstring& aName) { myName = aName; }
		int const& GetId() const { return myId; }
		void SetId(const int& aId) { myId = aId; }

	private:
		TextData myTextData = {};
		std::wstring myText = L"Text";
		std::wstring myName = L"Text";
		std::shared_ptr<Font> myFont;
		bool myIs2D;
		int myId;
};
