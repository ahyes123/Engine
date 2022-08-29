#pragma once
#include <cmath>
#include <cassert>
namespace CommonUtilities
{
	template <class T>
	class Vector2
	{
		public:
			T x;
			T y;

			Vector2<T>();
			Vector2<T>(const T& aX, const T& aY);
			Vector2<T>(const Vector2<T>& aVector);
			Vector2<T>& operator=(const Vector2<T>& aVector2);
			~Vector2<T>() = default;
			T LengthSqr() const;
			T Length() const;
			Vector2<T> GetNormalized() const;
			void Normalize();
			T Dot(const Vector2<T>& aVector) const;
	};

	template <class T> Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar);
	template <class T> Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector);
	template <class T> Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar);
	template <class T> void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1);
	template <class T> void operator*=(Vector2<T>& aVector, const T& aScalar);
	template <class T> void operator/=(Vector2<T>& aVector, const T& aScalar);

	template<class T>
	inline Vector2<T>::Vector2()
	{
		x = 0;
		y = 0;
	}
	template<class T>
	inline Vector2<T>::Vector2(const T& aX, const T& aY)
	{
		x = aX;
		y = aY;
	}
	template<class T>
	inline Vector2<T>::Vector2(const Vector2<T>& aVector)
	{
		x = aVector.x;
		y = aVector.y;
	}
	template<class T>
	inline T Vector2<T>::LengthSqr() const
	{
		return (x * x + y * y);
	}
	template<class T>
	inline T Vector2<T>::Length() const
	{
		return sqrt(LengthSqr());
	}
	template<class T>
	inline Vector2<T> Vector2<T>::GetNormalized() const
	{
		assert(Length() != 0 && "Tried to Normalize a zero vector. Division with 0!");

		T inversedLength = 1 / Length();
		return Vector2<T>(x * inversedLength, y * inversedLength);	
	}
	template<class T>
	inline void Vector2<T>::Normalize()
	{
		assert(Length() != 0 && "Tried to Normalize a zero vector. Division with 0!");

		T inversedLength = 1 / Length();
		x *= inversedLength;
		y *= inversedLength;
	}
	template<class T>
	inline T Vector2<T>::Dot(const Vector2<T>& aVector) const
	{
		T dot = ((x * aVector.x) + (y * aVector.y));
		return dot;
	}
	template <class T>
	Vector2<T>& Vector2<T>::operator=(const Vector2<T>& aVector)
	{
		x = aVector.x;
		y = aVector.y;

		return(*this);
	}
	template<class T>
	Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y);
	}
	template<class T>
	Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		return Vector2<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y);
	}
	template<class T>
	Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
	}
	template<class T>
	Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
	{
		return Vector2<T>(aVector * aScalar);
	}
	template<class T>
	Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0 && "Tried to divide by zero");
		T inversedScalar = 1 / aScalar;
		return Vector2<T>(aVector.x * inversedScalar, aVector.y * inversedScalar);
	}
	template<class T>
	void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
	}
	template<class T>
	void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
	}
	template<class T>
	void operator*=(Vector2<T>& aVector0, const T& aScalar)
	{
		aVector0.x *= aScalar;
		aVector0.y *= aScalar;
	}
	template<class T>
	void operator/=(Vector2<T>& aVector0, const T& aScalar)
	{
		assert(aScalar != 0 && "Tried to divide by zero");

		T inversedScalar = 1 / aScalar;
		aVector0.x *= inversedScalar;
		aVector0.y *= inversedScalar;
	}
}

using Vector2f = CommonUtilities::Vector2<float>;
