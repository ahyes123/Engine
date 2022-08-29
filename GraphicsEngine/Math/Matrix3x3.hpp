#pragma once
#include "Matrix4x4.hpp"
#include "Vector.hpp"
#include <math.h>

namespace CommonUtilities
{
	template <class T>
	class Matrix4x4;

	template <class T>
	class Matrix3x3
	{
	public:
		// Creates the identity matrix.
		Matrix3x3<T>();
		// Copy Constructor.
		Matrix3x3<T>(const Matrix3x3<T>& aMatrix);
		// Copies the top left 3x3 part of the Matrix4x4.
		Matrix3x3<T>(const Matrix4x4<T>& aMatrix);
		// () operator for accessing element (row, column) for read/write or read, respectively.
		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;
		// Static functions for creating rotation matrices.
		static Matrix3x3<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix3x3<T> CreateRotationAroundZ(T aAngleInRadians);
		// Static function for creating a transpose of a matrix.
		static Matrix3x3<T> Transpose(const Matrix3x3<T>& aMatrixToTranspose);
		Matrix3x3<T>& operator=(Matrix4x4<T>& aMatrix);
		Matrix3x3<T> operator*(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T> operator+(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T> operator-(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>& operator+=(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>& operator-=(const Matrix3x3<T>& aMatrix);
		Matrix3x3<T>& operator*=(const Matrix3x3<T>& aMatrix);

	private:
		T myMatrix[3][3] = { {} };
	};

	template<class T> bool operator==(const Matrix3x3<T>& aMatrixOne, const Matrix3x3<T>& aMatrixTwo);
	template<class T> Vector3<T>& operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix);

	template<class T>
	inline Matrix3x3<T>::Matrix3x3()
	{
		for (unsigned int row = 0; row < 3; ++row)
		{
			for (unsigned int column = 0; column < 3; ++column)
			{
				myMatrix[row][column] = static_cast<T>(0);
				if (row == column)
				{
					myMatrix[row][column] = static_cast<T>(1);
				}
			}
		}
	}
	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix3x3<T>& aMatrix)
	{
		*this = aMatrix;
	}
	template<class T>
	inline Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
	{
		for (unsigned int row = 0; row < 3; ++row)
		{
			for (unsigned int column = 0; column < 3; ++column)
			{
				myMatrix[row][column] = aMatrix(row, column);
			}
		}
	}
	template<class T>
	inline T& Matrix3x3<T>::operator()(const int aRow, const int aColumn)
	{
		return myMatrix[aRow - 1][aColumn - 1];
	}
	template<class T>
	inline const T& Matrix3x3<T>::operator()(const int aRow, const int aColumn) const
	{
		return myMatrix[aRow - 1][aColumn - 1];
	}
	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix3x3<T> ret;
		ret(0, 0) = 1;
		ret(0, 1) = 0;
		ret(0, 2) = 0;
		ret(1, 0) = 0;
		ret(2, 0) = 0;
		ret(2, 2) = cos(aAngleInRadians);
		ret(2, 3) = sin(aAngleInRadians);
		ret(3, 2) = -sin(aAngleInRadians);
		ret(3, 3) = cos(aAngleInRadians);
		return ret;
	}
	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		return Matrix3x3<T>();
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		return Matrix3x3<T>();
	}


	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::Transpose(const Matrix3x3<T>& aMatrixToTranspose)
	{
		return Matrix3x3<T>();
	}

	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator=(Matrix4x4<T>& aMatrix)
	{
		myMatrix[0][0] = aMatrix(0, 0);
		myMatrix[0][1] = aMatrix(0, 1);
		myMatrix[0][2] = aMatrix(0, 2);
		myMatrix[1][0] = aMatrix(1, 0);
		myMatrix[1][1] = aMatrix(1, 1);
		myMatrix[1][2] = aMatrix(1, 2);
		myMatrix[2][0] = aMatrix(2, 0);
		myMatrix[2][1] = aMatrix(2, 1);
		myMatrix[2][2] = aMatrix(2, 2);
		return(*this);
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3<T>& aMatrix)
	{
		Matrix3x3<T> temp;
		temp(0, 0) = myMatrix[0][0] * aMatrix(0, 0) + myMatrix[0][1] * aMatrix(1, 0) + myMatrix[0][2] * aMatrix(2, 0);
		temp(0, 1) = myMatrix[0][0] * aMatrix(0, 1) + myMatrix[0][1] * aMatrix(1, 1) + myMatrix[0][2] * aMatrix(2, 1);
		temp(0, 2) = myMatrix[0][0] * aMatrix(0, 2) + myMatrix[0][1] * aMatrix(1, 2) + myMatrix[0][2] * aMatrix(2, 2);
		temp(1, 0) = myMatrix[1][0] * aMatrix(0, 0) + myMatrix[1][1] * aMatrix(1, 0) + myMatrix[1][2] * aMatrix(2, 0);
		temp(1, 1) = myMatrix[1][0] * aMatrix(0, 1) + myMatrix[1][1] * aMatrix(1, 1) + myMatrix[1][2] * aMatrix(2, 1);
		temp(1, 2) = myMatrix[1][0] * aMatrix(0, 2) + myMatrix[1][1] * aMatrix(1, 2) + myMatrix[1][2] * aMatrix(2, 2);
		temp(2, 0) = myMatrix[2][0] * aMatrix(0, 0) + myMatrix[1][2] * aMatrix(1, 0) + myMatrix[2][2] * aMatrix(2, 0);
		temp(2, 1) = myMatrix[2][0] * aMatrix(0, 1) + myMatrix[1][2] * aMatrix(1, 1) + myMatrix[2][2] * aMatrix(2, 2);
		temp(2, 2) = myMatrix[2][0] * aMatrix(0, 2) + myMatrix[1][2] * aMatrix(1, 2) + myMatrix[2][2] * aMatrix(2, 2);

		return temp;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3<T>& aMatrix)
	{
		Matrix3x3<T> mat1 = aMatrix;

		for (unsigned int row = 0; row < 3; ++row)
		{
			for (unsigned int column = 0; column < 3; ++column)
			{
				mat1.myMatrix[row][column] += myMatrix[row][column];
			}
		}

		return mat1;
	}

	template<class T>
	inline Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3<T>& aMatrix)
	{
		myMatrix[0][0] -= aMatrix.myMatrix[0][0]; 
		myMatrix[1][0] -= aMatrix.myMatrix[1][0];
		myMatrix[2][0] -= aMatrix.myMatrix[2][0]; 
		myMatrix[0][1] -= aMatrix.myMatrix[0][1];
		myMatrix[1][1] -= aMatrix.myMatrix[1][1]; 
		myMatrix[2][1] -= aMatrix.myMatrix[2][1];
		myMatrix[0][2] -= aMatrix.myMatrix[0][2]; 
		myMatrix[1][2] -= aMatrix.myMatrix[1][2];
		myMatrix[2][2] -= aMatrix.myMatrix[2][2];
		return *this;
	}
	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator+=(const Matrix3x3<T>& aMatrix)
	{
		Matrix3x3<T> mat1 = aMatrix;

		for (unsigned int row = 0; row < 3; ++row)
		{
			for (unsigned int column = 0; column < 3; ++column)
			{
				myMatrix[row][column] += mat1.myMatrix[row][column];
			}
		}

		return *this;
	}
	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator-=(const Matrix3x3<T>& aMatrix)
	{
		myMatrix[0][0] -= aMatrix(0, 0);
		myMatrix[1][0] -= aMatrix(1, 0);
		myMatrix[2][0] -= aMatrix(2, 0);
		myMatrix[0][1] -= aMatrix(0, 1);
		myMatrix[1][1] -= aMatrix(1, 1);
		myMatrix[2][1] -= aMatrix(2, 1);
		myMatrix[0][2] -= aMatrix(0, 2);
		myMatrix[1][2] -= aMatrix(1, 2);
		myMatrix[2][2] -= aMatrix(2, 2);
		return *this;
	}
	template<class T>
	inline Matrix3x3<T>& Matrix3x3<T>::operator*=(const Matrix3x3<T>& aMatrix)
	{
		Matrix3x3<T> temp;
		temp(0, 0) = myMatrix[0][0] * aMatrix(0, 0) + myMatrix[1][0] * aMatrix(0, 1) + myMatrix[2][0] * aMatrix(0, 2);
		temp(0, 1) = myMatrix[0][0] * aMatrix(1, 0) + myMatrix[1][0] * aMatrix(1, 1) + myMatrix[2][0] * aMatrix(1, 2);
		temp(0, 2) = myMatrix[0][0] * aMatrix(2, 0) + myMatrix[1][0] * aMatrix(2, 1) + myMatrix[2][0] * aMatrix(2, 2);
		temp(1, 0) = myMatrix[0][1] * aMatrix(0, 0) + myMatrix[1][1] * aMatrix(0, 1) + myMatrix[2][1] * aMatrix(0, 2);
		temp(1, 1) = myMatrix[0][1] * aMatrix(1, 0) + myMatrix[1][1] * aMatrix(1, 1) + myMatrix[2][1] * aMatrix(1, 2);
		temp(1, 2) = myMatrix[0][1] * aMatrix(2, 0) + myMatrix[1][1] * aMatrix(2, 1) + myMatrix[2][1] * aMatrix(2, 2);
		temp(2, 0) = myMatrix[0][2] * aMatrix(0, 0) + myMatrix[1][2] * aMatrix(0, 1) + myMatrix[2][2] * aMatrix(0, 2);
		temp(2, 1) = myMatrix[0][2] * aMatrix(1, 0) + myMatrix[1][2] * aMatrix(1, 1) + myMatrix[2][2] * aMatrix(1, 2);
		temp(2, 2) = myMatrix[0][2] * aMatrix(2, 0) + myMatrix[1][2] * aMatrix(2, 1) + myMatrix[2][2] * aMatrix(2, 2);

		return *this;
	}
	template<class T>
	bool operator==(const Matrix3x3<T>& aMatrixOne, const Matrix3x3<T>& aMatrixTwo)
	{
		return false;
	}
	template<class T>
	Vector3<T>& operator*(const Vector3<T>& aVector, const Matrix3x3<T>& aMatrix)
	{
		Vector3<T> temp;

		temp.x = aVector.x * aMatrix(0, 0) + aVector.y * aMatrix(0, 1) + aVector.z * aMatrix(0, 2);
		temp.y = aVector.x * aMatrix(1, 0) + aVector.y * aMatrix(1, 1) + aVector.z * aMatrix(1, 2);
		temp.z = aVector.x * aMatrix(2, 0) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(2, 2);

		return temp;
	}
}