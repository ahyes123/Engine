#include "ShaderStructs.hlsli"
#include "ParticleStructs.hlsli"

SamplerState defaultSampler : register(s0);
Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D materialTexture : register(t2);
TextureCube environmentTexture : register(t10);

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