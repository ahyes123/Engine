#include "ShaderStructs.hlsli"

SamplerState defaultSampler : register(s0);
Texture2D TextureSlot1 : register(t30);

PostProcessPixelOutput main(PostProcessVertexToPixel input)
{
	PostProcessPixelOutput result;
	float4 color = TextureSlot1.Sample(defaultSampler, input.UV);

	if(color.a < 0.05f)
	{
		discard;
		result.Color = 0;
		return result;
	}
	result.Color.rgb = TextureSlot1.Sample(defaultSampler, input.UV).rgb;
	result.Color.a = 1.0f;
	return result;
}