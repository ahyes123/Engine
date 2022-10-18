#include "ShaderStructs.hlsli"

SamplerState defaultSampler : register(s0);
SamplerState wrapSampler : register(s1);
SamplerState pointClampSampler : register(s2);
SamplerState pointWrapSampler : register(s3);

Texture2D TextureSlot1 : register(t0);
Texture2D TextureSlot2 : register(t1);
Texture2D TextureSlot3 : register(t2);
Texture2D TextureSlot4 : register(t3);
Texture2D TextureSlot5 : register(t4);
Texture2D TextureSlot6 : register(t5);
Texture2D TextureSlot7 : register(t6);
Texture2D TextureSlot8 : register(t7);
Texture2D TextureSlot9 : register(t8);
Texture2D TextureSlot10 : register(t9);
Texture2D TextureSlot11 : register(t10);
Texture2D TextureSlot12 : register(t11);

float4 GetViewPosition(float2 uv)
{
	const float4 worldPosition = float4(TextureSlot5.Sample(defaultSampler, uv).rgb, 1);
	const float4 viewPosition = mul(FB_ToView, worldPosition);
	return viewPosition;
}

float4 GetViewNormal(float2 uv)
{
	const float4 worldNormal = float4(TextureSlot2.Sample(defaultSampler, uv).rgb, 0);
	const float4 viewNormal = mul(FB_ToView, worldNormal);
	return viewNormal;
}

float2 GetRandom(float2 uv, float2 uvScale)
{
	const float3 random = 2.0f * TextureSlot9.Sample(pointWrapSampler, uv * uvScale).rgb - 1.f;

	return random.xy;
}

float SSAO(float2 screenUV, float2 uv, float3 pxViewPos, float3 pxViewNrm, float scale, float bias, float intensity)
{
	const float3 diff = GetViewPosition(screenUV + uv).xyz - pxViewPos;
	const float3 v = normalize(diff);
	const float d = length(diff) * scale;
	const float occ = max(0.f, dot(pxViewNrm, v) - bias) * 1.f / (1.f + d) * intensity;
	return occ;
}

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput result;
	const float intensity = 0.35f;
	const float scale = 0.05f;
	const float bias = 0.05f;
	const float radius = 0.002f;
	const float offset = 0.707f;

	int noiseWidth = 0; 
	int noiseHeight = 0;
	int numMips = 0;
	TextureSlot9.GetDimensions(0, noiseWidth, noiseHeight, numMips);
	const float2 randomUVScale = FB_Resolution / float2(noiseWidth, noiseHeight);
	const float2 random = GetRandom(input.UV, randomUVScale);
	const float4 pxPos = GetViewPosition(input.UV);
	const float4 pxNrm = GetViewNormal(input.UV);
	const float2 frustrumCorners[4] = { float2(1, 0), float2(-1, 0), float2(0, 1), float2(0, -1) };

	float occlusion = 0.f;
	for(uint i = 0; i < 4; i++)
	{
		const uint idx = i % 4;
		const float2 coord1 = reflect(frustrumCorners[idx], random) * radius;
		const float2 coord2 = float2(coord1.x * offset - coord1.y * offset, coord1.x * offset + coord1.y * offset);

		occlusion += SSAO(input.UV, coord1 * 0.25f, pxPos.xyz, pxNrm.xyz, scale, bias, intensity);
		occlusion += SSAO(input.UV, coord2 * 0.5f, pxPos.xyz, pxNrm.xyz, scale, bias, intensity);
		occlusion += SSAO(input.UV, coord1 * 0.75f, pxPos.xyz, pxNrm.xyz, scale, bias, intensity);
		occlusion += SSAO(input.UV, coord2, pxPos.xyz, pxNrm.xyz, scale, bias, intensity);
	}
	result.Color.rgb = 1 - occlusion;
	result.Color.a = 1;
	return result;
}