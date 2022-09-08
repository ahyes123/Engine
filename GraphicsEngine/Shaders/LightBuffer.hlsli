struct LightData
{
	float3 Color;
	float Intensity;
	float3 Direction;
	float Range;
	float3 Position;
	float Attenuation;
	float SpotInnerRadius;
	float SpotOuterRadius;
	uint LightType;
	float padding;
};

cbuffer LightBuffer : register(b3)
{
	LightData LB_DirectionalLight;
	LightData LB_Lights[MAX_LIGHTS];
	uint LB_NumLights;
	float3 LB_Padding;
}