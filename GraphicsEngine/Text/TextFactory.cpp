#include "GraphicsEngine.pch.h"
#include "TextFactory.h"
#include <fstream>

#include "../External/Tool/DDSTextureLoader11.h"
#include "DX11.h"
#include "../External/nloman/json.hpp"
#include "Vertex.hpp"
#include "../Scene/SceneHandler.h"

std::vector<std::shared_ptr<Text>> TextFactory::myTextObjects;
std::unordered_map<std::wstring, Font> TextFactory::myFonts;

void TextFactory::Init()
{
	std::wstring fontFileName = L"Fonts/cgothic";
	const static::std::wstring atlasFileName = fontFileName + L".dds";
	const static::std::wstring fontDefinition = fontFileName + L".json";

	std::ifstream fontDefStream(fontDefinition);
	using nlohmann::json;
	json fontDef;
	fontDefStream >> fontDef;
	fontDefStream.close();

	Font aFont;

	aFont.Atlas.Size = fontDef["atlas"]["size"];
	aFont.Atlas.Width = fontDef["atlas"]["width"];
	aFont.Atlas.Height = fontDef["atlas"]["height"];
	aFont.Atlas.EmSize = fontDef["metrics"]["emSize"];
	aFont.Atlas.LineHeight = fontDef["metrics"]["lineHeight"];
	aFont.Atlas.Ascender = fontDef["metrics"]["ascender"];
	aFont.Atlas.Descender = fontDef["metrics"]["descender"];

	size_t glyphCount = fontDef["glyphs"].size();

	for(size_t g = 0; g < glyphCount; g++)
	{
		unsigned int unicode = fontDef["glyphs"][g].value("unicode", 0);
		float advance = fontDef["glyphs"][g].value("advance", -1.0f);
		Vector4f planeBounds;

		if(fontDef["glyphs"][g].find("planeBounds") != fontDef["glyphs"][g].end())
		{
			planeBounds.x = fontDef["glyphs"][g]["planeBounds"]["left"];
			planeBounds.y = fontDef["glyphs"][g]["planeBounds"]["bottom"];
			planeBounds.z = fontDef["glyphs"][g]["planeBounds"]["right"];
			planeBounds.w = fontDef["glyphs"][g]["planeBounds"]["top"];
		}

		Vector4f uvBounds;
		if(fontDef["glyphs"][g].find("atlasBounds") != fontDef["glyphs"][g].end())
		{
			const float UVStartX = static_cast<float>(fontDef["glyphs"][g]["atlasBounds"]["left"]) / static_cast<float>(aFont.Atlas.Width);
			const float UVStartY = static_cast<float>(fontDef["glyphs"][g]["atlasBounds"]["top"]) / static_cast<float>(aFont.Atlas.Height);

			const float UVEndX = static_cast<float>(fontDef["glyphs"][g]["atlasBounds"]["right"]) / static_cast<float>(aFont.Atlas.Width);
			const float UVEndY = static_cast<float>(fontDef["glyphs"][g]["atlasBounds"]["bottom"]) / static_cast<float>(aFont.Atlas.Height);

			uvBounds = { UVStartX, UVStartY, UVEndX, UVEndY };
		}

		aFont.Glyphs.insert({ unicode, {static_cast<char>(unicode), advance, planeBounds, uvBounds} });
	}

	const HRESULT createResult = DirectX::CreateDDSTextureFromFile(DX11::Device.Get(), atlasFileName.c_str(), aFont.myTexture.GetAddressOf(), aFont.SRV.GetAddressOf());

	if (SUCCEEDED(createResult))
	{
		myFonts.insert({ fontFileName, aFont });
	}
}

Font TextFactory::GetFont(std::wstring aFontName)
{
	auto It = myFonts.find(aFontName);
	return It->second;
}

std::shared_ptr<Text> TextFactory::CreateText(const std::wstring& someText, const int someWorldSize, const int aFontSize, const bool aIs2D)
{
	const std::string asciiString = std::string(someText.begin(), someText.end());

	std::wstring fontName = L"Fonts/cgothic";
	auto It = myFonts.find(fontName);
	Font font = It->second;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	float X = 0;
	float actualWorldSize = static_cast<float>(someWorldSize) / 1.f;
	font.Atlas.Size = aFontSize;
	
	for (auto c : asciiString)
	{
		float charAdvance = font[c].Advance;
		const unsigned int currentVertCount = static_cast<unsigned>(vertices.size());

		const Vector4f bounds = font[c].UVBounds;
		Vector4f offset = font[c].PlaneBounds;

		if (!aIs2D)
		{
			actualWorldSize = static_cast<float>(font.Atlas.Size);
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
	vertexSubresourceData.pSysMem = &vertices[0];

	ID3D11Buffer* vertexBuffer;
	result = DX11::Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &vertexBuffer);

	if (FAILED(result))
	{
		return nullptr;
	}

	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = static_cast<UINT>(indices.size()) * static_cast<UINT>(sizeof(unsigned int));
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData{};
	indexSubresourceData.pSysMem = &indices[0];

	ID3D11Buffer* indexBuffer;
	result = DX11::Device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &indexBuffer);

	if (FAILED(result))
	{
		return nullptr;
	}

	Text::TextData textData;
	textData.myNumberOfVertices = static_cast<UINT>(vertices.size());
	textData.myVertexBuffer = vertexBuffer;
	textData.myNumber0fIndices = static_cast<UINT>(indices.size());
	textData.myIndexBuffer = indexBuffer;
	textData.myStride = sizeof(Vertex);
	textData.myOffset = 0;
	textData.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	std::shared_ptr<Text> text = std::make_shared<Text>();

	std::shared_ptr<Font> fontPtr = std::make_shared<Font>(font);
	text->Init(textData, someText, fontPtr, aIs2D);
	text->SetId(SceneHandler::GetActiveScene()->GetNextId());
	return text;
}