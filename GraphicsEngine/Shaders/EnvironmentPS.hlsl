#include "PBRFunctions.hlsli"

SamplerState defaultSampler : register(s0);

Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D materialTexture : register(t2);
Texture2D vertexNormalTexture : register(t3);
Texture2D worldPositionTexture : register(t4);
Texture2D ambientOcclusionTexture : register(t5);
TextureCube environmentTexture : register(t10);

DeferredPixelOutput main(DeferredVertexToPixel input)
{
	DeferredPixelOutput result;

	const float4 albedo = albedoTexture.Sample(defaultSampler, input.UV);
	if(albedo.a == 0)
	{
		discard;
		result.Color = float4(0, 0, 0, 0);
		return result;
	}

	float4 material = materialTexture.Sample(defaultSampler, input.UV);

	const float metalness = material.r;
	const float roughness = material.g;
	const float emissive = material.b;
	const float emissiveStr = material.a;


	const float3 normal = normalTexture.Sample(defaultSampler, input.UV).rgb;
	const float3 vertexNormal = vertexNormalTexture.Sample(defaultSampler, input.UV).rgb;
	const float4 worldPosition = worldPositionTexture.Sample(defaultSampler, input.UV);
	const float ambientOcclusion = ambientOcclusionTexture.Sample(defaultSampler, input.UV).r;
	const float3 toEye = normalize(FB_CamTranslation.xyz - vertexNormal.xyz);

	const float3 specularColor = lerp((float3)0.04f, albedo, metalness);
	const float3 diffuseColor = lerp((float3)0.00f, albedo, 1 - metalness);

	const float3 ambientLighting = EvaluateAmbience(
		defaultSampler,
		environmentTexture, normal, vertexNormal,
		toEye, roughness, ambientOcclusion, diffuseColor, specularColor);

	const float3 directLighting = EvaluateDirectionalLight(
		diffuseColor, specularColor, vertexNormal, roughness,
		LB_Color, LB_Intensity, -LB_Direction, toEye);

	result.Color.rgb = LinearToGamma(directLighting + ambientLighting);
	result.Color.a = 1;

#ifdef _DEBUG
	switch (FB_RenderMode)
	{
	case 0: //Default
		result.Color.rgb = LinearToGamma(directLighting + ambientLighting);
		result.Color.a = 1;
		break;
	case 1: //UV1
		result.Color = float4(input.UV.x, input.UV.y, 0, 1);
		break;
	case 3: //VertexNormal
	{
		float3 debugNormal = vertexNormal;
		const float signedLength = (debugNormal.r + debugNormal.g + debugNormal.b) / 3;

		if (signedLength < 0)
		{
			debugNormal = float3(1 - abs(debugNormal));
		}
		result.Color.rgb = debugNormal;
		result.Color.a = 1;
		break;
	}
	case 4: //PixelNormal
	{
		float3 debugNormal = normal;
		const float signedLength = (debugNormal.r + debugNormal.g + debugNormal.b) / 3;

		if (signedLength < 0)
		{
			debugNormal = float3(1 - abs(debugNormal));
		}
		result.Color.rgb = debugNormal;
		result.Color.a = 1;
		break;
	}
	case 5: //AlbedoMap
		result.Color = albedo;
		break;
	case 6: //NormalMap
	{
		const float3 normalMapDebug = normalTexture.Sample(defaultSampler, input.UV).agb;
		result.Color = float4(normalMapDebug.r, normalMapDebug.g, 1, 1);
		break;
	}
	case 7: //DiffuseLight
		result.Color.rgb = saturate(diffuseColor);
		result.Color.a = 1;
		break;
	case 9: //DirectionalOnly
		result.Color.rgb = saturate(directLighting);
		result.Color.a = 1;
		break;
	case 10: //AmbientOnly
		result.Color.rgb = saturate(ambientLighting);
		result.Color.a = 1;
		break;
	case 11: //Ambient Occlusion
		result.Color.rgb = ambientOcclusion;
		result.Color.a = 1;
		break;
	case 12: //Roughness
		result.Color.rgb = roughness;
		result.Color.a = 1;
		break;
	case 13: //Metalness
		result.Color.rgb = metalness;
		result.Color.a = 1;
		break;
	case 14: //Emissiveness
		result.Color.rgb = emissive * emissiveStr;
		result.Color.a = 1;
		break;
	default:
		break;
	}
#else
	result.Color.rgb = LinearToGamma(directLighting + ambientLighting);
	result.Color.a = 1;
#endif

	return result;
}