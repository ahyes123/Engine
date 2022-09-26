SamplerState pointClampSampler : register(s1);

struct LightData
{
	float4x4 View[6];
	float4x4 Projection;

	float3 Color;
	float Intensity;
	float3 Direction;
	float Range;
	float3 Position;
	float Attenuation;
	float SpotInnerRadius;
	float SpotOuterRadius;
	uint LightType;
	bool CastShadows;

	float NearPlane;
	float FarPlane;
	float2 padding;
};

cbuffer LightBuffer : register(b3)
{
	LightData LB_DirectionalLight;
	LightData LB_Lights[MAX_LIGHTS];
	uint LB_NumLights;
	float3 LB_Padding;
}

Texture2D dirLightShadowMap : register(t20);
Texture2D spotLightShadowMap : register(t21);
Texture2D pointLightShadowMap1 : register(t22);
Texture2D pointLightShadowMap2 : register(t23);
Texture2D pointLightShadowMap3 : register(t24);
Texture2D pointLightShadowMap4 : register(t25);
Texture2D pointLightShadowMap5 : register(t26);
Texture2D pointLightShadowMap6 : register(t27);