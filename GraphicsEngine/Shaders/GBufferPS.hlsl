#include "PBRFunctions.hlsli"

SamplerState defaultSampler : register(s0);
Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D materialTexture : register(t2);

TextureCube environmentTexture : register(t10);

GBufferOutput main(VertexToPixel input)
{
	GBufferOutput result;
	const float4 albedo = albedoTexture.Sample(defaultSampler, input.myUV).rgba;

	if(albedo.a <= 0.05f)
	{
		discard;
		result.Albedo = float4(0, 0, 0, 0);
		return result;
	}

	const float3 normalMap = normalTexture.Sample(defaultSampler, input.myUV).agb;
	const float4 material = materialTexture.Sample(defaultSampler, input.myUV);

	const float ambientOcclusion = normalMap.b;

	const float3x3 TBN = float3x3
		(
			normalize(input.myTangent),
			normalize(input.myBinormal),
			normalize(input.myNormal)
			);

	float3 pixelNormal = normalMap;

	pixelNormal.z = 0;
	pixelNormal = 2.0f * pixelNormal - 1;
	pixelNormal.z = sqrt(1 - saturate(pixelNormal.x + pixelNormal.x + pixelNormal.y + pixelNormal.y));
	pixelNormal = normalize(pixelNormal);
	pixelNormal = normalize(mul(pixelNormal, TBN));


	result.Albedo = albedo;
	result.Normal = float4(pixelNormal, 1.0f);
	result.Material = material;
	result.VertexNormal = float4(normalize(input.myNormal), 1);
	result.WorldPosition = input.myVxPosition;
	result.AmbientOcclusion = ambientOcclusion;

	return result;
}