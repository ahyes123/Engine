#include "ShaderStructs.hlsli"
#include "Common.hlsli"

SamplerState defaultSampler : register(s0);
Texture2D TextureSlot1 : register(t0);

float3 Tonemap_Reinhard2(float3 x) {
	const float L_white = 4.0;
	return (x * (1.0 + x / (L_white * L_white))) / (1.0 + x);
}

float3 Tonemap_ACES(float3 x) {
	// Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
	const float a = 2.51;
	const float b = 0.03;
	const float c = 2.43;
	const float d = 0.59;
	const float e = 0.14;
	return (x * (a * x + b)) / (x * (c * x + d) + e);
}

float3 Tonemap_UnrealEngine(float3 input)
{
	// Unreal 3, Documentation: "Color Grading"
	// Adapted to be close to Tonemap_ACES, with similar range
	// Gamma 2.2 correction is baked in, don't use with sRGB conversion!
	return input / (input + 0.155) * 1.019;
}

float3 Tonemap_Lottes(float3 x) {
	// Lottes 2016, "Advanced Techniques and Optimization of HDR Color Pipelines"
	const float a = 1.6;
	const float d = 0.977;
	const float hdrMax = 8.0;
	const float midIn = 0.18;
	const float midOut = 0.267;

	// Can be precomputed
	const float b =
		(-pow(midIn, a) + pow(hdrMax, a) * midOut) /
		((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);
	const float c =
		(pow(hdrMax, a * d) * pow(midIn, a) - pow(hdrMax, a) * pow(midIn, a * d) * midOut) /
		((pow(hdrMax, a * d) - pow(midIn, a * d)) * midOut);

	return pow(x, a) / (pow(x, a * d) * b + c);
}

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput result;

	const float3 color = TextureSlot1.Sample(defaultSampler, input.UV).rgb;
	const uint mode = 2;

	switch (mode)
	{
	default:
		result.Color.rgb = LinearToGamma(color);
		break;
	case 1:
		result.Color.rgb = LinearToGamma(Tonemap_Reinhard2(color));
		break;
	case 2:
		result.Color.rgb = Tonemap_UnrealEngine(color);
		break;
	case 3:
		result.Color.rgb = LinearToGamma(Tonemap_ACES(color));
		break;
	case 4:
		result.Color.rgb = LinearToGamma(Tonemap_Lottes(color));
		break;
	}

	result.Color.a = 1;

	return result;
}