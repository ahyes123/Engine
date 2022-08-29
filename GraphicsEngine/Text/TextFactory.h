#pragma once
#include "Text.h"
#include <vector>

class TextFactory
{
	static std::vector<std::shared_ptr<Text>> myTextObjects;
	static std::unordered_map<std::wstring, Font> myFonts;

	public:
		static void Init();
		static Font GetFont(std::wstring aFontName);
		static std::shared_ptr<Text> CreateText(const std::wstring& someText, const int someWorldSize, const int aFontSize = 16, const bool aIs2D = false);
};

