#include "ShaderStructs.hlsli"
#include "ParticleStructs.hlsli"

ParticlePixelOutput main(ParticleGeometryToPixel input)
{
	ParticlePixelOutput result;
	float4 textureColor = albedoTexture.Sample(defaultSampler, input.UV);

	/*if (textureColor.a <= 0.05f)
	{
		discard;
	}*/

	result.Color.rgba = textureColor.rgba * input.Color.rgba;
	return result;
}