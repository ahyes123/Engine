#pragma once
#include <cmath>
#include <cassert>

namespace CommonUtilities
{
	template <class T>
	class Vector3
	{
		public:
			T x;
			T y;
			T z;

			Vector3<T>();
			Vector3<T>(const T& aX, const T& aY, const T& aZ);
			Vector3<T>(const Vector3<T>& aVector);
			Vector3<T>& operator=(const Vector3<T>& aVector3);
			Vector3<T>& operator=(const T& aValue);
			~Vector3<T>() = default;
			T LengthSqr() const;
			T Length() const;
			Vector3<T> GetNormalized() const;
			void Normalize();
			T Dot(const Vector3<T>& aVector) const;
			Vector3<T> Cross(const Vector3<T>& aVector) const;
			static Vector3<T> Lerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const float aPercent);
	};

	template <class T> Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
	template <class T> Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1);
	template <class T> Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar);
	template <class T> Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector);
	template <class T> Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar);
	template <class T> void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1);
	template <class T> void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1);	
	template <class T> void operator*=(Vector3<T>& aVector, const T& aScalar);	
	template <class T> void operator/=(Vector3<T>& aVector, const T& aScalar);

	template<class T>
	inline Vector3<T>::Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	template<class T>
	inline Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ)
	{
		x = aX;
		y = aY;
		z = aZ;
	}
	template<class T>
	inline Vector3<T>::Vector3(const Vector3<T>& aVector)
	{
		x = aVector.x;
		y = aVector.y;
		z = aVector.z;
	}
	template<class T>
	inline T Vector3<T>::LengthSqr() const
	{
		return (x * x + y * y + z * z);
	}
	template<class T>
	inline T Vector3<T>::Length() const
	{
		return sqrt(LengthSqr());
	}
	template<class T>
	inline Vector3<T> Vector3<T>::GetNormalized() const
	{
		assert(Length() != 0 && "Tried to Normalize a zero vector. Division with 0!");

		T inversedLength = 1 / Length();
		return Vector3<T>(x * inversedLength, y * inversedLength, z * inversedLength);
	}
	template<class T>
	inline void Vector3<T>::Normalize()
	{
		assert(Length() != 0 && "Tried to Normalize a zero vector. Division with 0!");

		T inversedLength = 1 / Length();
		x *= inversedLength;
		y *= inversedLength;
		z *= inversedLength;
	}
	template<class T>
	inline T Vector3<T>::Dot(const Vector3<T>& aVector) const
	{
		return ((x * aVector.x) + (y * aVector.y) + (z * aVector.z));
	}
	template<class T>
	inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
	{
		Vector3<T> cross((y * aVector.z - z * aVector.y), (z * aVector.x - x * aVector.z), (x * aVector.y - y * aVector.x));
		return cross;
	}

	template <class T>
	Vector3<T> Vector3<T>::Lerp(const Vector3<T>& aStart, const Vector3<T>& aEnd, const float aPercent)
	{
		return (aStart + aPercent * (aEnd - aStart));
	}

	template <class TYPE>
	Vector3<TYPE>& Vector3<TYPE>::operator=(const Vector3<TYPE>& aVector)
	{
		x = aVector.x;
		y = aVector.y;
		z = aVector.z;

		return(*this);
	}

	template <class T>
	Vector3<T>& Vector3<T>::operator=(const T& aValue)
	{
		x = aValue;
		y = aValue;
		z = aValue;

		return *this;
	}

	template<class T>
	Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z);
	}
	template<class T>
	Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		return Vector3<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z);
	}
	template<class T>
	Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
	{
		return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
	}
	template<class T>
	Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
	{
		return Vector3<T>(aVector * aScalar);
	}
	template<class T>
	Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
	{
		assert(aScalar != 0 && "Tried to divide by zero");

		T inversedScalar = 1 / aScalar;

		return Vector3<T>(aVector.x * inversedScalar, aVector.y * inversedScalar, aVector.z * inversedScalar);
	}
	template<class T>
	void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
	}
	template<class T>
	void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
	}
	template<class T>
	void operator*=(Vector3<T>& aVector0, const T& aScalar)
	{
		aVector0.x *= aScalar;
		aVector0.y *= aScalar;
		aVector0.z *= aScalar;
	}
	template<class T>
	void operator/=(Vector3<T>& aVector0, const T& aScalar)
	{
		assert(aScalar != 0 && "Tried to divide by zero");

		T inversedScalar = 1 / aScalar;
		aVector0.x *= inversedScalar;
		aVector0.y *= inversedScalar;
		aVector0.z *= inversedScalar;
	}
}

using Vector3f = CommonUtilities::Vector3<float>;

