#pragma once
#include <cmath>
#include <cassert>

namespace CommonUtilities
{
	template <class T>
	class Vector4
	{
		public:
			T x;
			T y;
			T z;
			T w;

			Vector4<T>();
			Vector4<T>(const T& aX, const T& aY, const T& aZ, const T& aW);
			Vector4<T>(const Vector4<T>& aVector);
			Vector4<T>& operator=(const Vector4<T>& aVector4);

			~Vector4<T>() = default;
			T LengthSqr() const;
			T Length() const;
			Vector4<T> GetNormalized() const;
			void Normalize();
			T Dot(const Vector4<T>& aVector) const;
	};

	template <class T> Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1);
	template <class T> Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1);
	template <class T> Vector4<T> operator-(const Vector4<T>& aVector0, const float& aValue);
	template <class T> Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar);
	template <class T> Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector);
	template <class T> Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar);
	template <class T> void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1);
	template <class T> void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1);
	template <class T> void operator*=(Vector4<T>& aVector, const T& aScalar);
	template <class T> void operator/=(Vector4<T>& aVector, const T& aScalar);

	template<class T>
	inline Vector4<T>::Vector4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}
	template<class T>
	inline Vector4<T>::Vector4(const T& aX, const T& aY, const T& aZ, const T& aW)
	{
		x = aX;
		y = aY;
		z = aZ;
		w = aW;
	}
	template<class T>
	inline Vector4<T>::Vector4(const Vector4<T>& aVector)
	{
		x = aVector.x;
		y = aVector.y;
		z = aVector.z;
		w = aVector.w;
	}
	template<class T>
	inline T Vector4<T>::LengthSqr() const
	{
		return (x * x + y * y + z * z + w * w);
	}
	template<class T>
	inline T Vector4<T>::Length() const
	{
		return sqrt(LengthSqr());
	}
	template<class T>
	inline Vector4<T> Vector4<T>::GetNormalized() const
	{
		assert(Length() != 0 && "Tried to Normalize a zero vector. Division by zero!");

		T inversedLength = 1 / Length();
		return Vector4<T>(x * inversedLength, y * inversedLength, z * inversedLength, w * inversedLength);
	}
	template<class T>
	inline void Vector4<T>::Normalize()
	{
		assert(Length() != 0 && "Tried to Normalize a zero vector. Division by zero!");

		T inversedLength = 1 / Length();
		x *= inversedLength;
		y *= inversedLength;
		z *= inversedLength;
		w *= inversedLength;
	}
	template<class T>
	inline T Vector4<T>::Dot(const Vector4<T>& aVector) const
	{
		return ((x * aVector.x) + (y * aVector.y) + (z * aVector.z) + (w * aVector.w));
	}

	template <class TYPE>
	Vector4<TYPE>& Vector4<TYPE>::operator=(const Vector4<TYPE>& aVector)
	{
		x = aVector.x;
		y = aVector.y;
		z = aVector.z;
		w = aVector.w;

		return(*this);
	}

	template<class T>
	Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return Vector4<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z, aVector0.w + aVector1.w);
	}
	template<class T>
	Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		return Vector4<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z, aVector0.w - aVector1.w);
	}

	template <class T>
	Vector4<T> operator-(const Vector4<T>& aVector0, const float& aValue)
	{
		return Vector4<T>(aVector0.x - aValue, aVector0.y - aValue, aVector0.z - aValue, aVector0.w - aValue);
	}

	template<class T>
	Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
	{
		return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
	}
	template<class T>
	Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
	{
		return Vector4<T>(aVector * aScalar);
	}
	template<class T>
	Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0 && "Tried to divide by zero");

		T inversedLength = 1.0f / aScalar;

		return Vector4<T>(aVector.x * inversedLength, aVector.y * inversedLength, aVector.z * inversedLength, aVector.w * inversedLength);
	}
	template<class T>
	void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
		aVector0.w += aVector1.w;
	}
	template<class T>
	void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
		aVector0.w -= aVector1.w;
	}
	template<class T>
	void operator*=(Vector4<T>& aVector0, const T& aScalar)
	{
		aVector0.x *= aScalar;
		aVector0.y *= aScalar;
		aVector0.z *= aScalar;
		aVector0.w *= aScalar;
	}
	template<class T>
	void operator/=(Vector4<T>& aVector0, const T& aScalar)
	{
		assert(aScalar != 0 && "Tried to divide by zero");

		T inversedLength = 1.0f / aScalar;

		aVector0.x *= inversedLength;
		aVector0.y *= inversedLength;
		aVector0.z *= inversedLength;
		aVector0.w *= inversedLength;
	}
}

using Vector4f = CommonUtilities::Vector4<float>;

