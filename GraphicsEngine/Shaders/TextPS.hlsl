#include "TextStructs.hlsli"

float median(float3 rgb)
{
	return max(min(rgb.r, rgb.g), min(max(rgb.r, rgb.g), rgb.b));
}

float2 safeNormalize(in float2 v)
{
	float len = length(v);
	len = (len > 0.0) ? 1.0 / len : 0.0;
	return v * len;
}

float ScreenPxRange(float2 uvs)
{
	const float screenPxRange = 2;
	float2 unitRange = float2(screenPxRange, screenPxRange) / float2(212, 212);
	float2 screenTexSize = float2(1.0, 1.0) / fwidth(uvs);
	return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

PixelOutput main(VertexToPixel input)
{
	PixelOutput result;

	const float3 fontPx = fontTexture.Sample(defaultSampler, input.myUV).rgb;

	float sd = median(fontPx);
	float screenPxDist = ScreenPxRange(input.myUV) * (sd - 0.5);
	float opacity = clamp(screenPxDist + 0.5, 0.0, 1.0);

	float bgColor = float3(0, 0, 0);
	float3 fgColor = float3(1, 1, 1);

	result.myColor.rgb = lerp(bgColor, fgColor, opacity);
	result.myColor.a = (result.myColor.r + result.myColor.g, result.myColor.b) > 0.05f;

	return result;
}