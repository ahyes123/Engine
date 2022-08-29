#pragma once
#include <assert.h>

namespace CommonUtilities
{
	template<typename T>
	T Max(T aFirst, T aSecond)
	{
		if(aFirst < aSecond)
		{
			return aSecond;
		}
		return aFirst;
	}

	template<typename T>
	T Min(T aFirst, T aSecond)
	{
		if (aFirst < aSecond)
		{
			return aFirst;
		}
		return aSecond;
	}

	template<typename T>
	T Abs(T aValue)
	{
		if(aValue > 0)
		{
			return aValue;
		}
		return aValue * -1;
	}

	template<typename T>
	T Clamp(T aMin, T aMax, T aValue)
	{
		assert(aMin <= aMax && "Min is greater than Max");

		if(aValue < aMin)
		{
			return aMin;
		}
		if(aValue > aMax)
		{
			return aMax;
		}
		return aValue;
	}

	template<typename T>
	T Lerp(T aStart, T aEnd, float aSpeed)
	{
		return aStart + aSpeed * (aEnd - aStart);
	}

	template<typename T>
	void Swap(T& aFirst, T& aSecond)
	{
		T tempVariable = aFirst;
		aFirst = aSecond;
		aSecond = tempVariable;
	}
}

namespace Helper
{
	inline std::wstring CleanModelName(const std::wstring& aModelName)
	{
		unsigned int endIndex = aModelName.rfind('.');

		unsigned int index = aModelName.rfind('/');

		std::wstring name = aModelName.substr(index + 1, endIndex - 7);

		std::wstring ending = name;

		index = ending.rfind('_');

		if (ending.substr(index, 3) == std::wstring(L"_sk") ||
			ending.substr(index, 3) == std::wstring(L"_SK"))
		{
			name = ending.substr(0, index);
		}

		return name;
		//unsigned int index = aModelName.rfind('.');

		//std::wstring name = aModelName.substr(0, index);

		//index = name.rfind('_');

		//if (aModelName.substr(index, 3) == std::wstring(L"_sk") ||
		//	aModelName.substr(index, 3) == std::wstring(L"_SK"))
		//{
		//	name = aModelName.substr(0, index);
		//}

		//return name;
	}
}
