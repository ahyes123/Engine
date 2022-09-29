#include "ShaderStructs.hlsli"

SamplerState defaultSampler : register(s0);
Texture2D TextureSlot1 : register(t30);
Texture2D TextureSlot2 : register(t31);

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput result;

	const float3 resource1 = TextureSlot1.Sample(defaultSampler, input.UV).rgb;
	const float3 resource2 = TextureSlot2.Sample(defaultSampler, input.UV).rgb;

	{
		//const float luminance = dot(resource1, float3(0.2126f, 0.7152f, 0.0722f));
		const float3 scaledResource = resource1 * (1.0f - saturate(resource2));
		result.Color.rgb = scaledResource + resource2;
		result.Color.a = 1.0f;
	}

	return result;
}