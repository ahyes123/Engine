#include "PBRFunctions.hlsli"

SamplerState defaultSampler : register(s0);

Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D materialTexture : register(t2);

TextureCube environmentTexture : register(t10);

PixelOutput main(VertexToPixel input)
{
	PixelOutput result;

	const float3x3 TBN = float3x3(
		normalize(input.myTangent),
		normalize(input.myBinormal),
		normalize(input.myNormal));

	float3 normalMap = normalTexture.Sample(defaultSampler, input.myUV).agr;
	float3 albedo = albedoTexture.Sample(defaultSampler, input.myUV).rgb;
	float4 material = materialTexture.Sample(defaultSampler, input.myUV);

	const float ambientOcclusion = normalMap.b;
	const float metalness = material.r;
	const float roughness = material.g;
	const float emissive = material.b;
	const float emissiveStr = material.a;

	float3 pixelNormal = normalMap;
	pixelNormal.z = 0;
	pixelNormal = 2.0f * pixelNormal - 1;
	pixelNormal.z = sqrt(1 - saturate(pixelNormal.x + pixelNormal.x + pixelNormal.y + pixelNormal.y));
	pixelNormal = normalize(pixelNormal);
	pixelNormal = normalize(mul(pixelNormal, TBN));

	const float3 toEye = normalize(FB_CamTranslation.xyz - input.myVxPosition.xyz);
	const float3 specularColor = lerp((float3)0.04f, albedo, metalness);
	const float3 diffuseColor = lerp((float3)0.00f, albedo, 1 - metalness);

	const float3 ambientLighting = EvaluateAmbience(defaultSampler,
		environmentTexture, pixelNormal, input.myNormal, 
		toEye, roughness, ambientOcclusion, diffuseColor, specularColor);

	const float3 directLighting = EvaluateDirectionalLight(
		diffuseColor, specularColor, pixelNormal, roughness,
		LB_Color, LB_Intensity, -LB_Direction, toEye);

	//const float3 L = -1 * normalize(LB_Direction);
	//const float3 N = pixelNormal;
	//const float LdotN = saturate(dot(L, N));
	//const float3 C = LB_Color;
	//const float Ilight = LB_Intensity;

	//const float3 Ipixel = LdotN * C * Ilight;

	//const float3 environment = environmentTexture.SampleLevel(defaultSampler, input.myNormal, 5).rgb;

	const float3 diffuse = albedo * directLighting;
	const float3 ambient = albedo * ambientLighting;

#ifdef _DEBUG
	switch (FB_RenderMode)
	{
		case 0: //Default
			result.myColor.rgb = LinearToGamma(directLighting + ambientLighting);
			result.myColor.a = 1;
			break;
		case 1: //UV1
			result.myColor = float4(input.myUV.x, input.myUV.y, 0, 1);
			break;
		case 2: //VertexColor
			result.myColor = float4(input.myVxColor.r, input.myVxColor.g, input.myVxColor.b, input.myVxColor.a);
			break;
		case 3: //VertexNormal
		{
			float3 debugNormal = input.myNormal;
			const float signedLength = (debugNormal.r + debugNormal.g + debugNormal.b) / 3;

			if (signedLength < 0)
			{
				debugNormal = float3(1 - abs(debugNormal));
			}
			result.myColor.rgb = debugNormal;
			result.myColor.a = 1;
			break;
		}
		case 4: //PixelNormal
		{
			float3 debugNormal = pixelNormal;
			const float signedLength = (debugNormal.r + debugNormal.g + debugNormal.b) / 3;

			if (signedLength < 0)
			{
				debugNormal = float3(1 - abs(debugNormal));
			}
			result.myColor.rgb = debugNormal;
			result.myColor.a = 1;
			break;
		}
		case 5: //AlbedoMap
			result.myColor = float4(albedo, 1);
			break;
		case 6: //NormalMap
		{
			const float3 normalMapDebug = normalTexture.Sample(defaultSampler, input.myUV).agb;
			result.myColor = float4(normalMapDebug.r, normalMapDebug.g, 1, 1);
			break;
		}
		case 7: //DiffuseLight
			result.myColor.rgb = saturate(diffuse);
			result.myColor.a = 1;
			break;
		case 8: //AmbientLight
			result.myColor.rgb = saturate(ambient);
			result.myColor.a = 1;
			break;
		case 9: //DirectionalOnly
			result.myColor.rgb = saturate(directLighting);
			result.myColor.a = 1;
			break;
		case 10: //AmbientOnly
			result.myColor.rgb = saturate(ambientLighting);
			result.myColor.a = 1;
			break;
		case 11: //Ambient Occlusion
			result.myColor.rgb = ambientOcclusion;
			result.myColor.a = 1;
			break;
		case 12: //Roughness
			result.myColor.rgb = roughness;
			result.myColor.a = 1;
			break;
		case 13: //Metalness
			result.myColor.rgb = metalness;
			result.myColor.a = 1;
			break;
		case 14: //Emissiveness
			result.myColor.rgb = emissive * emissiveStr;
			result.myColor.a = 1;
			break;
		default:
			break;
	}
#else
	result.myColor.rgb = LinearToGamma(directLighting + ambientLighting);
	result.myColor.a = 1;
#endif
	return result;
}