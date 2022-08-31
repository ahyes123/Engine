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