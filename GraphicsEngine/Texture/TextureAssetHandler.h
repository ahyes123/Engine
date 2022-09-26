#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "DepthStencil.h"
#include "GBuffer.h"
#include "Vector2.hpp"

class Texture;

class TextureAssetHandler
{
	typedef std::unordered_map<std::wstring, std::shared_ptr<Texture>> TextureMap;

	inline static TextureMap myRegistry{};

	public:
		static std::shared_ptr<Texture> GetTexture(const std::wstring& aName);

		static bool LoadTexture(const std::wstring& aFileName);

		static std::unique_ptr<GBuffer> CreateGBuffer(RECT aWindowSize);

		static std::shared_ptr<DepthStencil> CreateDepthStencil(const std::wstring& aName, size_t aWidth,
																size_t aHeight);
};

