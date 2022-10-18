#include "TextStructs.hlsli"

VertexToPixel main(VertexInput input)
{
	VertexToPixel result;

	result.myBinormal = input.myBinormal;
	result.myNormal = input.myNormal;
	result.myTangent = input.myTangent;
	result.myUV = input.myUV;
	result.myUV2 = input.myUV2;
	result.myUV3 = input.myUV3;
	result.myUV4 = input.myUV4;
	result.myVxColor = input.myVxColor;
	result.myVxColor2 = input.myVxColor2;
	result.myVxColor3 = input.myVxColor3;
	result.myVxColor4 = input.myVxColor4;

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