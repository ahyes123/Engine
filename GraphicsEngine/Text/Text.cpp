#include "GraphicsEngine.pch.h"
#include "Text.h"
#include "TextFactory.h"
#include <Model/Vertex.hpp>
#include <Engine/DX11.cpp>

void Text::Init(TextData someTextData, std::wstring someText, std::shared_ptr<Font> aFont, bool aIs2D)
{
	myTextData = someTextData;
	myText = someText;
	myFont = aFont;
	myIs2D = aIs2D;
}

void Text::SetText(const std::wstring someText)
{
	const std::string asciiString = std::string(someText.begin(), someText.end());
	const size_t strLen = strlen(asciiString.c_str());

	std::wstring fontName = L"Fonts/cgothic";
	Font font = TextFactory::GetFont(fontName);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	float X = 0;
	float actualWorldSize = static_cast<float>(1) / 1.f;
	font.Atlas.Size = myFont->Atlas.Size;

	for (auto c : asciiString)
	{
		float charAdvance = font[c].Advance;
		const unsigned int currentVertCount = static_cast<unsigned>(vertices.size());

		const Vector4f bounds = font[c].UVBounds;
		Vector4f offset = font[c].PlaneBounds;

		if (!myIs2D)
		{
			actualWorldSize = font.Atlas.Size;
			charAdvance *= actualWorldSize;

			offset.x = font[c].PlaneBounds.x * charAdvance;
			offset.z = font[c].PlaneBounds.z * actualWorldSize * 0.65f;
			offset.y = font[c].PlaneBounds.y * charAdvance;
			offset.w = font[c].PlaneBounds.w * actualWorldSize;

			if (abs(offset.w) > 0.005f)
				offset.w += font.Atlas.Descender * actualWorldSize;
		}
		else
		{
			actualWorldSize = (float)font.Atlas.Size / 100.f;
			charAdvance *= actualWorldSize * 0.60f;

			offset.x = font[c].PlaneBounds.x * charAdvance;
			offset.z = font[c].PlaneBounds.z * actualWorldSize * 0.35f;
			offset.y = font[c].PlaneBounds.y * charAdvance;
			offset.w = font[c].PlaneBounds.w * actualWorldSize;

			if (abs(offset.w) > 0.005f)
				offset.w += font.Atlas.Descender * actualWorldSize;
		}

		vertices.push_back({ X + offset.x, offset.y, 0, 1,
			1, 1, 1, 1 });
		vertices.back().UVs[0] = { bounds.x, 1 - bounds.w };

		vertices.push_back({ X + offset.x, offset.w, 0, 1,
			1, 1, 1, 1 });
		vertices.back().UVs[0] = { bounds.x, 1 - bounds.y };

		vertices.push_back({ X + offset.z, offset.y, 0, 1,
			1, 1, 1, 1 });
		vertices.back().UVs[0] = { bounds.z, 1 - bounds.w };

		vertices.push_back({ X + offset.z, offset.w, 0, 1,
			1, 1, 1, 1 });
		vertices.back().UVs[0] = { bounds.z, 1 - bounds.y };

		X += charAdvance * 0.65f;

		indices.push_back(currentVertCount);
		indices.push_back(currentVertCount + 1);
		indices.push_back(currentVertCount + 2);
		indices.push_back(currentVertCount + 1);
		indices.push_back(currentVertCount + 3);
		indices.push_back(currentVertCount + 2);
	}

	HRESULT result;

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(vertices.size()) * static_cast<UINT>(sizeof(Vertex));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	if (vertices.size() > 0)
	{
		vertexSubresourceData.pSysMem = &vertices[0];
	}



	ID3D11Buffer* vertexBuffer;
	result = DX11::Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &vertexBuffer);

	if (FAILED(result))
	{

	}

	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = static_cast<UINT>(indices.size()) * static_cast<UINT>(sizeof(unsigned int));
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData{};
	if (indices.size() > 0)
		indexSubresourceData.pSysMem = &indices[0];

	ID3D11Buffer* indexBuffer;
	result = DX11::Device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &indexBuffer);

	if (FAILED(result))
	{

	}

	myTextData.myNumberOfVertices = static_cast<UINT>(vertices.size());
	myTextData.myVertexBuffer = vertexBuffer;
	myTextData.myNumber0fIndices = static_cast<UINT>(indices.size());
	myTextData.myIndexBuffer = indexBuffer;
	myTextData.myStride = sizeof(Vertex);
	myTextData.myOffset = 0;
	myTextData.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::shared_ptr<Font> fontPtr = std::make_shared<Font>(font);
	myText = someText;
	myFont = fontPtr;
}