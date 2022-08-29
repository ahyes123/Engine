#pragma once
#include <unordered_map>
#include <wrl.h>
#include "Vector4.hpp"
#include <d3d11.h>

using namespace Microsoft::WRL;

struct Font
{
	struct Atlas
	{
		int Size;
		int Width;
		int Height;
		float EmSize;
		float LineHeight;
		float Ascender;
		float Descender;
		float UnderlineY;
		float UnderlineThickness;
	} Atlas;

	struct Glyph
	{
		char Character;
		float Advance;
		Vector4f PlaneBounds;
		Vector4f UVBounds;
	};

	std::unordered_map<unsigned int, Glyph> Glyphs;
	ComPtr<ID3D11Resource> myTexture;
	ComPtr<ID3D11ShaderResourceView> SRV;

	Glyph operator[](char c);
};

inline Font::Glyph Font::operator[](char c)
{
	for (int i = 0; i < Glyphs.size(); i++)
	{
		if (Glyphs[i].Character == c)
			return Glyphs[i];
	}

	Glyph glyph;
	return glyph;
}

