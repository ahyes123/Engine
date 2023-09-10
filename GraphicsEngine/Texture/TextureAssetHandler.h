#pragma once
#include <unordered_map>

#include "DepthStencil.h"
#include "GBuffer.h"
#include "RenderTarget.h"
#include "Vector2.hpp"

class Texture;

class TextureAssetHandler
{
	typedef std::unordered_map<std::string, std::shared_ptr<Texture>> TextureMap;

	inline static TextureMap myRegistry{};

	public:
		static std::shared_ptr<Texture> GetTexture(const std::string& aName);

		static bool LoadTexture(const std::string& aFileName);

		static std::unique_ptr<GBuffer> CreateGBuffer(RECT aWindowSize);

		static std::shared_ptr<DepthStencil> CreateDepthStencil(const std::wstring& aName, size_t aWidth,
																size_t aHeight);

		static std::unique_ptr<RenderTarget> CreateRenderTarget(UINT aWidth, UINT aHeight, DXGI_FORMAT aFormat);

};

