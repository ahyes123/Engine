#pragma once
#include <initializer_list>
#include "Vector.hpp"
#include "Math/Vector4.hpp"

struct Vertex
{
	Vertex() = default;

	Vertex(std::initializer_list<float> aList)
	{
		auto iterator = aList.begin();
		int i = 0;
		for (const auto& element : aList)
		{
			switch (i)
			{
				case 0:
					Position.x = element;
					break;
				case 1:
					Position.y = element;
					break;
				case 2:
					Position.z = element;
					break;
				case 3:
					Position.w = element;
					break;
				case 4:
					VertexColors[0].x = element;
					break;
				case 5:
					VertexColors[0].y = element;
					break;
				case 6:
					VertexColors[0].z = element;
					break;
				case 7:
					VertexColors[0].w = element;
					break;
			}
			i++;
		}
	}

	Vector4f Position = { 0.0f, 0.0f, 0.0f, 1.0f };

	Vector4f VertexColors[4] =
	{
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	};

	Vector2f UVs[4]
	{
		{0, 0},
		{0, 0},
		{0, 0},
		{0, 0}
	};

	CommonUtilities::Vector4<unsigned int> myBoneIDs = { 0, 0, 0, 0 };
	Vector4f myBoneWeights = { 0, 0, 0, 0 };

	Vector3f Tangent = { 0, 0, 0 };
	Vector3f Binormal = { 0, 0, 0 };
	Vector3f Normal = { 0, 0, 0 };
};

struct ParticleVertex
{
	Vector4f Position = { 0, 0, 0, 1 };
	Vector4f Color = { 0, 0, 0, 0 };
	Vector3f Velocity = { 0, 0, 0 };
	Vector3f Scale = { 1, 1, 1 };
	float LifeTime = 0;
};

