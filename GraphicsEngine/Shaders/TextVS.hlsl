#include "TextStructs.hlsli"

VertexToPixel main(VertexInput input)
{
	VertexToPixel result;

	result.myVxColor = input.myVxColor;
	result.myUV = input.myUV;

	float4 vertexObjectPos = input.myPosition;
	float4 vertexWorldPos = mul(OB_ToWorld, vertexObjectPos);
	if (OB_2D)
	{
		result.myPosition = vertexWorldPos;
		return result;
	}

	float4 vertexViewPos = mul(FB_ToCamera, vertexWorldPos);
	float4 vertexProjectionPos = mul(FB_ToProjection, vertexViewPos);

	result.myPosition = vertexProjectionPos;

	return result;
}