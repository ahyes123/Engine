#pragma once
#include "Vector.hpp"
#include <array>
#include "FBXImporterStructs.h"
#include <iostream>

#define PI 3.14159265358979323846f
#define RAD2DEG(a) (a * 180.f / PI)

namespace CommonUtilities
{
	template<class T>
	class Matrix4x4
	{
	public:
		Matrix4x4<T>();
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix);

		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;
		Matrix4x4<T> operator=(const Matrix4x4& aMatrix4x4);
		Matrix4x4<T> operator=(const TGA::Matrix& aMatrix4x4);
		void Decompose(Vector3f& outPos, Vector3f& outRot, Vector3f& outScale);

		static Matrix4x4<T> CreateScaleMatrix(Vector3<T> aScalarVector);
		static Matrix4x4<T> CreateTranslationMatrix(Vector3<T> aTranslationVector);

		static Matrix4x4<T> CreateRotationAroundX(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(T aAngleInRadians);
		static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose);
		static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform);

		static Vector3<T> DecomposeRotation(const Matrix4x4<T>& aMatrix);

		void Deconstruct(Vector3<T>& aPosition, Vector3<T>& aRotation, Vector3<T>& aScale);

	private:
		std::array<std::array<T, 4>, 4> myMatrix;
	};

	template <class T>
	Matrix4x4<T>::Matrix4x4()
	{
		myMatrix[0][0] = 1;
		myMatrix[0][1] = 0;
		myMatrix[0][2] = 0;
		myMatrix[0][3] = 0;

		myMatrix[1][0] = 0;
		myMatrix[1][1] = 1;
		myMatrix[1][2] = 0;
		myMatrix[1][3] = 0;

		myMatrix[2][0] = 0;
		myMatrix[2][1] = 0;
		myMatrix[2][2] = 1;
		myMatrix[2][3] = 0;

		myMatrix[3][0] = 0;
		myMatrix[3][1] = 0;
		myMatrix[3][2] = 0;
		myMatrix[3][3] = 1;
	}

	template <class T>
	Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix)
	{
		myMatrix[0][0] = aMatrix(1, 1);
		myMatrix[0][1] = aMatrix(1, 2);
		myMatrix[0][2] = aMatrix(1, 3);
		myMatrix[0][3] = aMatrix(1, 4);

		myMatrix[1][0] = aMatrix(2, 1);
		myMatrix[1][1] = aMatrix(2, 2);
		myMatrix[1][2] = aMatrix(2, 3);
		myMatrix[1][3] = aMatrix(2, 4);

		myMatrix[2][0] = aMatrix(3, 1);
		myMatrix[2][1] = aMatrix(3, 2);
		myMatrix[2][2] = aMatrix(3, 3);
		myMatrix[2][3] = aMatrix(3, 4);

		myMatrix[3][0] = aMatrix(4, 1);
		myMatrix[3][1] = aMatrix(4, 2);
		myMatrix[3][2] = aMatrix(4, 3);
		myMatrix[3][3] = aMatrix(4, 4);
	}

	template<class T>
	void Matrix4x4<T>::Deconstruct(Vector3<T>& aPosition, Vector3<T>& aRotation, Vector3<T>& aScale)
	{
		const Matrix4x4<T>& mat = *this;
		Vector4<T> scaleX = { mat(1,1), mat(1,2) , mat(1,3) , mat(1,4) };
		Vector4<T> scaleY = { mat(2,1), mat(2,2) , mat(2,3) , mat(2,4) };
		Vector4<T> scaleZ = { mat(3,1), mat(3,2) , mat(3,3) , mat(3,4) };

		aScale.x = scaleX.Length();
		aScale.y = scaleY.Length();
		aScale.z = scaleZ.Length();

		scaleX.Normalize();
		scaleY.Normalize();
		scaleZ.Normalize();

		aRotation.x = atan2f(scaleY.z, scaleZ.z);
		aRotation.y = atan2f(-scaleX.z, sqrtf(scaleY.z * scaleY.z + scaleZ.z * scaleZ.z));
		aRotation.z = atan2f(scaleX.y, scaleX.x);


		aPosition.x = mat(4, 1);
		aPosition.y = mat(4, 2);
		aPosition.z = mat(4, 3);
	}

	template <class T>
	T& Matrix4x4<T>::operator()(const int aRow, const int aColumn)
	{
		assert(aRow <= 4 && aColumn <= 4 && aRow > 0 && aColumn > 0);
		return myMatrix[aRow - 1][aColumn - 1];
	}

	template <class T>
	const T& Matrix4x4<T>::operator()(const int aRow, const int aColumn) const
	{
		assert(aRow <= 4 && aColumn <= 4 && aRow > 0 && aColumn > 0);
		return myMatrix[aRow - 1][aColumn - 1];
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::operator=(const Matrix4x4& aMatrix4x4)
	{
		myMatrix[0][0] = aMatrix4x4(1, 1);
		myMatrix[0][1] = aMatrix4x4(1, 2);
		myMatrix[0][2] = aMatrix4x4(1, 3);
		myMatrix[0][3] = aMatrix4x4(1, 4);

		myMatrix[1][0] = aMatrix4x4(2, 1);
		myMatrix[1][1] = aMatrix4x4(2, 2);
		myMatrix[1][2] = aMatrix4x4(2, 3);
		myMatrix[1][3] = aMatrix4x4(2, 4);

		myMatrix[2][0] = aMatrix4x4(3, 1);
		myMatrix[2][1] = aMatrix4x4(3, 2);
		myMatrix[2][2] = aMatrix4x4(3, 3);
		myMatrix[2][3] = aMatrix4x4(3, 4);

		myMatrix[3][0] = aMatrix4x4(4, 1);
		myMatrix[3][1] = aMatrix4x4(4, 2);
		myMatrix[3][2] = aMatrix4x4(4, 3);
		myMatrix[3][3] = aMatrix4x4(4, 4);

		return *this;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::operator=(const TGA::Matrix& aMatrix4x4)
	{
		myMatrix[0][0] = aMatrix4x4.Data[0];
		myMatrix[0][1] = aMatrix4x4.Data[1];
		myMatrix[0][2] = aMatrix4x4.Data[2];
		myMatrix[0][3] = aMatrix4x4.Data[3];

		myMatrix[1][0] = aMatrix4x4.Data[4];
		myMatrix[1][1] = aMatrix4x4.Data[5];
		myMatrix[1][2] = aMatrix4x4.Data[6];
		myMatrix[1][3] = aMatrix4x4.Data[7];

		myMatrix[2][0] = aMatrix4x4.Data[8];
		myMatrix[2][1] = aMatrix4x4.Data[9];
		myMatrix[2][2] = aMatrix4x4.Data[10];
		myMatrix[2][3] = aMatrix4x4.Data[11];

		myMatrix[3][0] = aMatrix4x4.Data[12];
		myMatrix[3][1] = aMatrix4x4.Data[13];
		myMatrix[3][2] = aMatrix4x4.Data[14];
		myMatrix[3][3] = aMatrix4x4.Data[15];

		return *this;

	}

	template <class T>
	void Matrix4x4<T>::Decompose(Vector3f& outPos, Vector3f& outRot, Vector3f& outScale)
	{
		outPos = { myMatrix[3][0], myMatrix[3][1], myMatrix[3][2] };

		Vector3<T> rowOne = Vector3<T>(myMatrix[0][0], myMatrix[0][1], myMatrix[0][2]);
		Vector3<T> rowTwo = Vector3<T>(myMatrix[1][0], myMatrix[1][1], myMatrix[1][2]);
		Vector3<T> rowThree = Vector3<T>(myMatrix[2][0], myMatrix[2][1], myMatrix[2][2]);

		T scaleX = rowOne.Length();
		T scaleY = rowTwo.Length();
		T scaleZ = rowThree.Length();

		outScale = { scaleX, scaleY, scaleZ };

		rowOne /= scaleX;
		rowTwo /= scaleY;
		rowThree /= scaleZ;

		outRot.x = atan2(rowThree.y, rowThree.z);
		outRot.y = atan2(-rowThree.x, sqrt(rowThree.y * rowThree.y + rowThree.z * rowThree.z));
		outRot.z = atan2(rowTwo.x, rowOne.x);
		outRot = RAD2DEG(outRot);
		outRot *= -1.f;
	}

	template <class T>
	bool operator==(const Matrix4x4<T>& aMatrix4x4Zero, const Matrix4x4<T>& aMatrix4x4One)
	{
		for (int i = 1; i < 5; i++)
		{
			for (int j = 1; j < 5; j++)
			{
				if (aMatrix4x4Zero(i, j) != aMatrix4x4One(i, j))
				{
					return false;
				}
			}
		}

		return true;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateScaleMatrix(Vector3<T> aScaleVector)
	{
		Matrix4x4<T> result;
		result(1, 1) = aScaleVector.x;
		result(2, 2) = aScaleVector.y;
		result(3, 3) = aScaleVector.z;
		return result;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateTranslationMatrix(Vector3<T> aTranslationVector)
	{
		Matrix4x4<T> result;

		result(4, 1) = aTranslationVector.x;
		result(4, 2) = aTranslationVector.y;
		result(4, 3) = aTranslationVector.z;

		return result;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(T aAngleInRadians)
	{
		Matrix4x4<T> tempMatrix4x4;

		tempMatrix4x4(1, 1) = 1;
		tempMatrix4x4(1, 2) = 0;
		tempMatrix4x4(1, 3) = 0;
		tempMatrix4x4(1, 4) = 0;

		tempMatrix4x4(2, 1) = 0;
		tempMatrix4x4(2, 2) = cos(aAngleInRadians);
		tempMatrix4x4(2, 3) = sin(aAngleInRadians);
		tempMatrix4x4(2, 4) = 0;

		tempMatrix4x4(3, 1) = 0;
		tempMatrix4x4(3, 2) = -sin(aAngleInRadians);
		tempMatrix4x4(3, 3) = cos(aAngleInRadians);
		tempMatrix4x4(3, 4) = 0;

		tempMatrix4x4(4, 1) = 0;
		tempMatrix4x4(4, 2) = 0;
		tempMatrix4x4(4, 3) = 0;
		tempMatrix4x4(4, 4) = 1;

		return tempMatrix4x4;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(T aAngleInRadians)
	{
		Matrix4x4<T> tempMatrix4x4;

		tempMatrix4x4(1, 1) = cos(aAngleInRadians);;
		tempMatrix4x4(1, 2) = 0;
		tempMatrix4x4(1, 3) = -sin(aAngleInRadians);
		tempMatrix4x4(1, 4) = 0;

		tempMatrix4x4(2, 1) = 0;
		tempMatrix4x4(2, 2) = 1;
		tempMatrix4x4(2, 3) = 0;
		tempMatrix4x4(2, 4) = 0;

		tempMatrix4x4(3, 1) = sin(aAngleInRadians);
		tempMatrix4x4(3, 2) = 0;
		tempMatrix4x4(3, 3) = cos(aAngleInRadians);
		tempMatrix4x4(3, 4) = 0;

		tempMatrix4x4(4, 1) = 0;
		tempMatrix4x4(4, 2) = 0;
		tempMatrix4x4(4, 3) = 0;
		tempMatrix4x4(4, 4) = 1;

		return tempMatrix4x4;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(T aAngleInRadians)
	{
		Matrix4x4<T> tempMatrix4x4;

		tempMatrix4x4(1, 1) = cos(aAngleInRadians);
		tempMatrix4x4(1, 2) = sin(aAngleInRadians);
		tempMatrix4x4(1, 3) = 0;
		tempMatrix4x4(1, 4) = 0;

		tempMatrix4x4(2, 1) = -sin(aAngleInRadians);
		tempMatrix4x4(2, 2) = cos(aAngleInRadians);
		tempMatrix4x4(2, 3) = 0;
		tempMatrix4x4(2, 4) = 0;

		tempMatrix4x4(3, 1) = 0;
		tempMatrix4x4(3, 2) = 0;
		tempMatrix4x4(3, 3) = 1;
		tempMatrix4x4(3, 4) = 0;

		tempMatrix4x4(4, 1) = 0;
		tempMatrix4x4(4, 2) = 0;
		tempMatrix4x4(4, 3) = 0;
		tempMatrix4x4(4, 4) = 1;

		return tempMatrix4x4;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T>& aMatrixToTranspose)
	{
		Matrix4x4<T> tempMatrix4x4;

		tempMatrix4x4(1, 1) = aMatrixToTranspose(1, 1);
		tempMatrix4x4(1, 2) = aMatrixToTranspose(2, 1);
		tempMatrix4x4(1, 3) = aMatrixToTranspose(3, 1);
		tempMatrix4x4(1, 4) = aMatrixToTranspose(4, 1);

		tempMatrix4x4(2, 1) = aMatrixToTranspose(1, 2);
		tempMatrix4x4(2, 2) = aMatrixToTranspose(2, 2);
		tempMatrix4x4(2, 3) = aMatrixToTranspose(3, 2);
		tempMatrix4x4(2, 4) = aMatrixToTranspose(4, 2);

		tempMatrix4x4(3, 1) = aMatrixToTranspose(1, 3);
		tempMatrix4x4(3, 2) = aMatrixToTranspose(2, 3);
		tempMatrix4x4(3, 3) = aMatrixToTranspose(3, 3);
		tempMatrix4x4(3, 4) = aMatrixToTranspose(4, 3);

		tempMatrix4x4(4, 1) = aMatrixToTranspose(1, 4);
		tempMatrix4x4(4, 2) = aMatrixToTranspose(2, 4);
		tempMatrix4x4(4, 3) = aMatrixToTranspose(3, 4);
		tempMatrix4x4(4, 4) = aMatrixToTranspose(4, 4);

		return tempMatrix4x4;
	}

	template <class T>
	Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aTransform)
	{
		Matrix4x4<T> tempRot;
		tempRot(1, 1) = aTransform(1, 1);
		tempRot(1, 2) = aTransform(2, 1);
		tempRot(1, 3) = aTransform(3, 1);

		tempRot(2, 1) = aTransform(1, 2);
		tempRot(2, 2) = aTransform(2, 2);
		tempRot(2, 3) = aTransform(3, 2);

		tempRot(3, 1) = aTransform(1, 3);
		tempRot(3, 2) = aTransform(2, 3);
		tempRot(3, 3) = aTransform(3, 3);

		Matrix4x4<T> tempPos;
		tempPos(4, 1) = aTransform(4, 1) * -1;
		tempPos(4, 2) = aTransform(4, 2) * -1;
		tempPos(4, 3) = aTransform(4, 3) * -1;

		return tempPos * tempRot;
	}

	template <class T>
	Vector3<T> Matrix4x4<T>::DecomposeRotation(const Matrix4x4<T>& aMatrix)
	{
		Vector3<T> vector;

		return vector;
	}

	template <class T>
	Matrix4x4<T> operator+(Matrix4x4<T>& aMatrix4x4Zero, const Matrix4x4<T>& aMatrix4x4One)
	{
		Matrix4x4<T> tempMatrix;
		tempMatrix(1, 1) = aMatrix4x4Zero(1, 1) + aMatrix4x4One(1, 1);
		tempMatrix(1, 2) = aMatrix4x4Zero(1, 2) + aMatrix4x4One(1, 2);
		tempMatrix(1, 3) = aMatrix4x4Zero(1, 3) + aMatrix4x4One(1, 3);
		tempMatrix(1, 4) = aMatrix4x4Zero(1, 4) + aMatrix4x4One(1, 4);

		tempMatrix(2, 1) = aMatrix4x4Zero(2, 1) + aMatrix4x4One(2, 1);
		tempMatrix(2, 2) = aMatrix4x4Zero(2, 2) + aMatrix4x4One(2, 2);
		tempMatrix(2, 3) = aMatrix4x4Zero(2, 3) + aMatrix4x4One(2, 3);
		tempMatrix(2, 4) = aMatrix4x4Zero(2, 4) + aMatrix4x4One(2, 4);

		tempMatrix(3, 1) = aMatrix4x4Zero(3, 1) + aMatrix4x4One(3, 1);
		tempMatrix(3, 2) = aMatrix4x4Zero(3, 2) + aMatrix4x4One(3, 2);
		tempMatrix(3, 3) = aMatrix4x4Zero(3, 3) + aMatrix4x4One(3, 3);
		tempMatrix(3, 4) = aMatrix4x4Zero(3, 4) + aMatrix4x4One(3, 4);

		tempMatrix(4, 1) = aMatrix4x4Zero(4, 1) + aMatrix4x4One(4, 1);
		tempMatrix(4, 2) = aMatrix4x4Zero(4, 2) + aMatrix4x4One(4, 2);
		tempMatrix(4, 3) = aMatrix4x4Zero(4, 3) + aMatrix4x4One(4, 3);
		tempMatrix(4, 4) = aMatrix4x4Zero(4, 4) + aMatrix4x4One(4, 4);

		return tempMatrix;
	}

	template <class T>
	void operator+=(Matrix4x4<T>& aMatrix4x4Zero, const Matrix4x4<T>& aMatrix4x4One)
	{
		aMatrix4x4Zero(1, 1) += aMatrix4x4One(1, 1);
		aMatrix4x4Zero(1, 2) += aMatrix4x4One(1, 2);
		aMatrix4x4Zero(1, 3) += aMatrix4x4One(1, 3);
		aMatrix4x4Zero(1, 4) += aMatrix4x4One(1, 4);

		aMatrix4x4Zero(2, 1) += aMatrix4x4One(2, 1);
		aMatrix4x4Zero(2, 2) += aMatrix4x4One(2, 2);
		aMatrix4x4Zero(2, 3) += aMatrix4x4One(2, 3);
		aMatrix4x4Zero(2, 4) += aMatrix4x4One(2, 4);

		aMatrix4x4Zero(3, 1) += aMatrix4x4One(3, 1);
		aMatrix4x4Zero(3, 2) += aMatrix4x4One(3, 2);
		aMatrix4x4Zero(3, 3) += aMatrix4x4One(3, 3);
		aMatrix4x4Zero(3, 4) += aMatrix4x4One(3, 4);

		aMatrix4x4Zero(4, 1) += aMatrix4x4One(4, 1);
		aMatrix4x4Zero(4, 2) += aMatrix4x4One(4, 2);
		aMatrix4x4Zero(4, 3) += aMatrix4x4One(4, 3);
		aMatrix4x4Zero(4, 4) += aMatrix4x4One(4, 4);
	}

	template <class T>
	Matrix4x4<T> operator-(Matrix4x4<T>& aMatrix4x4Zero, const Matrix4x4<T>& aMatrix4x4One)
	{
		Matrix4x4<T> tempMatrix;
		tempMatrix(1, 1) = aMatrix4x4Zero(1, 1) - aMatrix4x4One(1, 1);
		tempMatrix(1, 2) = aMatrix4x4Zero(1, 2) - aMatrix4x4One(1, 2);
		tempMatrix(1, 3) = aMatrix4x4Zero(1, 3) - aMatrix4x4One(1, 3);
		tempMatrix(1, 4) = aMatrix4x4Zero(1, 4) - aMatrix4x4One(1, 4);

		tempMatrix(2, 1) = aMatrix4x4Zero(2, 1) - aMatrix4x4One(2, 1);
		tempMatrix(2, 2) = aMatrix4x4Zero(2, 2) - aMatrix4x4One(2, 2);
		tempMatrix(2, 3) = aMatrix4x4Zero(2, 3) - aMatrix4x4One(2, 3);
		tempMatrix(2, 4) = aMatrix4x4Zero(2, 4) - aMatrix4x4One(2, 4);

		tempMatrix(3, 1) = aMatrix4x4Zero(3, 1) - aMatrix4x4One(3, 1);
		tempMatrix(3, 2) = aMatrix4x4Zero(3, 2) - aMatrix4x4One(3, 2);
		tempMatrix(3, 3) = aMatrix4x4Zero(3, 3) - aMatrix4x4One(3, 3);
		tempMatrix(3, 4) = aMatrix4x4Zero(3, 4) - aMatrix4x4One(3, 4);

		tempMatrix(4, 1) = aMatrix4x4Zero(4, 1) - aMatrix4x4One(4, 1);
		tempMatrix(4, 2) = aMatrix4x4Zero(4, 2) - aMatrix4x4One(4, 2);
		tempMatrix(4, 3) = aMatrix4x4Zero(4, 3) - aMatrix4x4One(4, 3);
		tempMatrix(4, 4) = aMatrix4x4Zero(4, 4) - aMatrix4x4One(4, 4);

		return tempMatrix;
	}

	template <class T>
	void operator-=(Matrix4x4<T>& aMatrix4x4Zero, const Matrix4x4<T>& aMatrix4x4One)
	{
		aMatrix4x4Zero(1, 1) -= aMatrix4x4One(1, 1);
		aMatrix4x4Zero(1, 2) -= aMatrix4x4One(1, 2);
		aMatrix4x4Zero(1, 3) -= aMatrix4x4One(1, 3);
		aMatrix4x4Zero(1, 4) -= aMatrix4x4One(1, 4);

		aMatrix4x4Zero(2, 1) -= aMatrix4x4One(2, 1);
		aMatrix4x4Zero(2, 2) -= aMatrix4x4One(2, 2);
		aMatrix4x4Zero(2, 3) -= aMatrix4x4One(2, 3);
		aMatrix4x4Zero(2, 4) -= aMatrix4x4One(2, 4);

		aMatrix4x4Zero(3, 1) -= aMatrix4x4One(3, 1);
		aMatrix4x4Zero(3, 2) -= aMatrix4x4One(3, 2);
		aMatrix4x4Zero(3, 3) -= aMatrix4x4One(3, 3);
		aMatrix4x4Zero(3, 4) -= aMatrix4x4One(3, 4);

		aMatrix4x4Zero(4, 1) -= aMatrix4x4One(4, 1);
		aMatrix4x4Zero(4, 2) -= aMatrix4x4One(4, 2);
		aMatrix4x4Zero(4, 3) -= aMatrix4x4One(4, 3);
		aMatrix4x4Zero(4, 4) -= aMatrix4x4One(4, 4);
	}

	template<class T>
	Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrix4x4Zero, const Matrix4x4<T>& aMatrix4x4One)
	{
		Matrix4x4<T> tempMatrix;

		tempMatrix(1, 1) = (aMatrix4x4Zero(1, 1) * aMatrix4x4One(1, 1)) + (aMatrix4x4Zero(1, 2) * aMatrix4x4One(2, 1)) + (aMatrix4x4Zero(1, 3) * aMatrix4x4One(3, 1)) + (aMatrix4x4Zero(1, 4) * aMatrix4x4One(4, 1));
		tempMatrix(1, 2) = (aMatrix4x4Zero(1, 1) * aMatrix4x4One(1, 2)) + (aMatrix4x4Zero(1, 2) * aMatrix4x4One(2, 2)) + (aMatrix4x4Zero(1, 3) * aMatrix4x4One(3, 2)) + (aMatrix4x4Zero(1, 4) * aMatrix4x4One(4, 2));
		tempMatrix(1, 3) = (aMatrix4x4Zero(1, 1) * aMatrix4x4One(1, 3)) + (aMatrix4x4Zero(1, 2) * aMatrix4x4One(2, 3)) + (aMatrix4x4Zero(1, 3) * aMatrix4x4One(3, 3)) + (aMatrix4x4Zero(1, 4) * aMatrix4x4One(4, 3));
		tempMatrix(1, 4) = (aMatrix4x4Zero(1, 1) * aMatrix4x4One(1, 4)) + (aMatrix4x4Zero(1, 2) * aMatrix4x4One(2, 4)) + (aMatrix4x4Zero(1, 3) * aMatrix4x4One(3, 4)) + (aMatrix4x4Zero(1, 4) * aMatrix4x4One(4, 4));

		tempMatrix(2, 1) = aMatrix4x4Zero(2, 1) * aMatrix4x4One(1, 1) + aMatrix4x4Zero(2, 2) * aMatrix4x4One(2, 1) + aMatrix4x4Zero(2, 3) * aMatrix4x4One(3, 1) + aMatrix4x4Zero(2, 4) * aMatrix4x4One(4, 1);
		tempMatrix(2, 2) = aMatrix4x4Zero(2, 1) * aMatrix4x4One(1, 2) + aMatrix4x4Zero(2, 2) * aMatrix4x4One(2, 2) + aMatrix4x4Zero(2, 3) * aMatrix4x4One(3, 2) + aMatrix4x4Zero(2, 4) * aMatrix4x4One(4, 2);
		tempMatrix(2, 3) = aMatrix4x4Zero(2, 1) * aMatrix4x4One(1, 3) + aMatrix4x4Zero(2, 2) * aMatrix4x4One(2, 3) + aMatrix4x4Zero(2, 3) * aMatrix4x4One(3, 3) + aMatrix4x4Zero(2, 4) * aMatrix4x4One(4, 3);
		tempMatrix(2, 4) = aMatrix4x4Zero(2, 1) * aMatrix4x4One(1, 4) + aMatrix4x4Zero(2, 2) * aMatrix4x4One(2, 4) + aMatrix4x4Zero(2, 3) * aMatrix4x4One(3, 4) + aMatrix4x4Zero(2, 4) * aMatrix4x4One(4, 4);

		tempMatrix(3, 1) = aMatrix4x4Zero(3, 1) * aMatrix4x4One(1, 1) + aMatrix4x4Zero(3, 2) * aMatrix4x4One(2, 1) + aMatrix4x4Zero(3, 3) * aMatrix4x4One(3, 1) + aMatrix4x4Zero(3, 4) * aMatrix4x4One(4, 1);
		tempMatrix(3, 2) = aMatrix4x4Zero(3, 1) * aMatrix4x4One(1, 2) + aMatrix4x4Zero(3, 2) * aMatrix4x4One(2, 2) + aMatrix4x4Zero(3, 3) * aMatrix4x4One(3, 2) + aMatrix4x4Zero(3, 4) * aMatrix4x4One(4, 2);
		tempMatrix(3, 3) = aMatrix4x4Zero(3, 1) * aMatrix4x4One(1, 3) + aMatrix4x4Zero(3, 2) * aMatrix4x4One(2, 3) + aMatrix4x4Zero(3, 3) * aMatrix4x4One(3, 3) + aMatrix4x4Zero(3, 4) * aMatrix4x4One(4, 3);
		tempMatrix(3, 4) = aMatrix4x4Zero(3, 1) * aMatrix4x4One(1, 4) + aMatrix4x4Zero(3, 2) * aMatrix4x4One(2, 4) + aMatrix4x4Zero(3, 3) * aMatrix4x4One(3, 4) + aMatrix4x4Zero(3, 4) * aMatrix4x4One(4, 4);

		tempMatrix(4, 1) = aMatrix4x4Zero(4, 1) * aMatrix4x4One(1, 1) + aMatrix4x4Zero(4, 2) * aMatrix4x4One(2, 1) + aMatrix4x4Zero(4, 3) * aMatrix4x4One(3, 1) + aMatrix4x4Zero(4, 4) * aMatrix4x4One(4, 1);
		tempMatrix(4, 2) = aMatrix4x4Zero(4, 1) * aMatrix4x4One(1, 2) + aMatrix4x4Zero(4, 2) * aMatrix4x4One(2, 2) + aMatrix4x4Zero(4, 3) * aMatrix4x4One(3, 2) + aMatrix4x4Zero(4, 4) * aMatrix4x4One(4, 2);
		tempMatrix(4, 3) = aMatrix4x4Zero(4, 1) * aMatrix4x4One(1, 3) + aMatrix4x4Zero(4, 2) * aMatrix4x4One(2, 3) + aMatrix4x4Zero(4, 3) * aMatrix4x4One(3, 3) + aMatrix4x4Zero(4, 4) * aMatrix4x4One(4, 3);
		tempMatrix(4, 4) = aMatrix4x4Zero(4, 1) * aMatrix4x4One(1, 4) + aMatrix4x4Zero(4, 2) * aMatrix4x4One(2, 4) + aMatrix4x4Zero(4, 3) * aMatrix4x4One(3, 4) + aMatrix4x4Zero(4, 4) * aMatrix4x4One(4, 4);

		return tempMatrix;
	}

	template<class T>
	void operator*=(Matrix4x4<T>& aMatrix4x4Zero, const Matrix4x4<T>& aMatrix4x4One)
	{
		Matrix4x4<T> tempMatrix;

		tempMatrix(1, 1) = (aMatrix4x4Zero(1, 1) * aMatrix4x4One(1, 1)) + (aMatrix4x4Zero(1, 2) * aMatrix4x4One(2, 1)) + (aMatrix4x4Zero(1, 3) * aMatrix4x4One(3, 1)) + (aMatrix4x4Zero(1, 4) * aMatrix4x4One(4, 1));
		tempMatrix(1, 2) = (aMatrix4x4Zero(1, 1) * aMatrix4x4One(1, 2)) + (aMatrix4x4Zero(1, 2) * aMatrix4x4One(2, 2)) + (aMatrix4x4Zero(1, 3) * aMatrix4x4One(3, 2)) + (aMatrix4x4Zero(1, 4) * aMatrix4x4One(4, 2));
		tempMatrix(1, 3) = (aMatrix4x4Zero(1, 1) * aMatrix4x4One(1, 3)) + (aMatrix4x4Zero(1, 2) * aMatrix4x4One(2, 3)) + (aMatrix4x4Zero(1, 3) * aMatrix4x4One(3, 3)) + (aMatrix4x4Zero(1, 4) * aMatrix4x4One(4, 3));
		tempMatrix(1, 4) = (aMatrix4x4Zero(1, 1) * aMatrix4x4One(1, 4)) + (aMatrix4x4Zero(1, 2) * aMatrix4x4One(2, 4)) + (aMatrix4x4Zero(1, 3) * aMatrix4x4One(3, 4)) + (aMatrix4x4Zero(1, 4) * aMatrix4x4One(4, 4));

		tempMatrix(2, 1) = aMatrix4x4Zero(2, 1) * aMatrix4x4One(1, 1) + aMatrix4x4Zero(2, 2) * aMatrix4x4One(2, 1) + aMatrix4x4Zero(2, 3) * aMatrix4x4One(3, 1) + aMatrix4x4Zero(2, 4) * aMatrix4x4One(4, 1);
		tempMatrix(2, 2) = aMatrix4x4Zero(2, 1) * aMatrix4x4One(1, 2) + aMatrix4x4Zero(2, 2) * aMatrix4x4One(2, 2) + aMatrix4x4Zero(2, 3) * aMatrix4x4One(3, 2) + aMatrix4x4Zero(2, 4) * aMatrix4x4One(4, 2);
		tempMatrix(2, 3) = aMatrix4x4Zero(2, 1) * aMatrix4x4One(1, 3) + aMatrix4x4Zero(2, 2) * aMatrix4x4One(2, 3) + aMatrix4x4Zero(2, 3) * aMatrix4x4One(3, 3) + aMatrix4x4Zero(2, 4) * aMatrix4x4One(4, 3);
		tempMatrix(2, 4) = aMatrix4x4Zero(2, 1) * aMatrix4x4One(1, 4) + aMatrix4x4Zero(2, 2) * aMatrix4x4One(2, 4) + aMatrix4x4Zero(2, 3) * aMatrix4x4One(3, 4) + aMatrix4x4Zero(2, 4) * aMatrix4x4One(4, 4);

		tempMatrix(3, 1) = aMatrix4x4Zero(3, 1) * aMatrix4x4One(1, 1) + aMatrix4x4Zero(3, 2) * aMatrix4x4One(2, 1) + aMatrix4x4Zero(3, 3) * aMatrix4x4One(3, 1) + aMatrix4x4Zero(3, 4) * aMatrix4x4One(4, 1);
		tempMatrix(3, 2) = aMatrix4x4Zero(3, 1) * aMatrix4x4One(1, 2) + aMatrix4x4Zero(3, 2) * aMatrix4x4One(2, 2) + aMatrix4x4Zero(3, 3) * aMatrix4x4One(3, 2) + aMatrix4x4Zero(3, 4) * aMatrix4x4One(4, 2);
		tempMatrix(3, 3) = aMatrix4x4Zero(3, 1) * aMatrix4x4One(1, 3) + aMatrix4x4Zero(3, 2) * aMatrix4x4One(2, 3) + aMatrix4x4Zero(3, 3) * aMatrix4x4One(3, 3) + aMatrix4x4Zero(3, 4) * aMatrix4x4One(4, 3);
		tempMatrix(3, 4) = aMatrix4x4Zero(3, 1) * aMatrix4x4One(1, 4) + aMatrix4x4Zero(3, 2) * aMatrix4x4One(2, 4) + aMatrix4x4Zero(3, 3) * aMatrix4x4One(3, 4) + aMatrix4x4Zero(3, 4) * aMatrix4x4One(4, 4);

		tempMatrix(4, 1) = aMatrix4x4Zero(4, 1) * aMatrix4x4One(1, 1) + aMatrix4x4Zero(4, 2) * aMatrix4x4One(2, 1) + aMatrix4x4Zero(4, 3) * aMatrix4x4One(3, 1) + aMatrix4x4Zero(4, 4) * aMatrix4x4One(4, 1);
		tempMatrix(4, 2) = aMatrix4x4Zero(4, 1) * aMatrix4x4One(1, 2) + aMatrix4x4Zero(4, 2) * aMatrix4x4One(2, 2) + aMatrix4x4Zero(4, 3) * aMatrix4x4One(3, 2) + aMatrix4x4Zero(4, 4) * aMatrix4x4One(4, 2);
		tempMatrix(4, 3) = aMatrix4x4Zero(4, 1) * aMatrix4x4One(1, 3) + aMatrix4x4Zero(4, 2) * aMatrix4x4One(2, 3) + aMatrix4x4Zero(4, 3) * aMatrix4x4One(3, 3) + aMatrix4x4Zero(4, 4) * aMatrix4x4One(4, 3);
		tempMatrix(4, 4) = aMatrix4x4Zero(4, 1) * aMatrix4x4One(1, 4) + aMatrix4x4Zero(4, 2) * aMatrix4x4One(2, 4) + aMatrix4x4Zero(4, 3) * aMatrix4x4One(3, 4) + aMatrix4x4Zero(4, 4) * aMatrix4x4One(4, 4);
		aMatrix4x4Zero = tempMatrix;
	}

	template <class T>
	Vector4<T> operator*(const Vector4<T>& aVector4, const Matrix4x4<T>& aMatrix4x4)
	{
		Vector4<T> tempVector;

		tempVector.x = aVector4.x * aMatrix4x4(1, 1) + aVector4.y * aMatrix4x4(2, 1) + aVector4.z * aMatrix4x4(3, 1) + aVector4.w * aMatrix4x4(4, 1);
		tempVector.y = aVector4.x * aMatrix4x4(1, 2) + aVector4.y * aMatrix4x4(2, 2) + aVector4.z * aMatrix4x4(3, 2) + aVector4.w * aMatrix4x4(4, 2);
		tempVector.z = aVector4.x * aMatrix4x4(1, 3) + aVector4.y * aMatrix4x4(2, 3) + aVector4.z * aMatrix4x4(3, 3) + aVector4.w * aMatrix4x4(4, 3);
		tempVector.w = aVector4.x * aMatrix4x4(1, 4) + aVector4.y * aMatrix4x4(2, 4) + aVector4.z * aMatrix4x4(3, 4) + aVector4.w * aMatrix4x4(4, 4);

		return tempVector;
	}
}

using Matrix4x4f = CommonUtilities::Matrix4x4<float>;

