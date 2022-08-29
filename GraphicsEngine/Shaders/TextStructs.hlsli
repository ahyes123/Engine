cbuffer FrameBuffer : register(b0)
{
	float4x4 FB_ToCamera;
	float4x4 FB_ToProjection;
	float FB_NearPlane;
	float FB_FarPlane;
	uint FB_RenderMode;
	float FB_padding;
}

cbuffer ObjectBuffer : register(b1)
{
	float4x4 OB_ToWorld;
	bool OB_2D;
	float3 OB_padding;
}

SamplerState defaultSampler : register(s0);

Texture2D fontTexture : register(t0);

struct VertexInput
{
	float4 myPosition : POSITION;
	float4 myVxColor : COLOR0;
	float4 myVxColor2 : COLOR1;
	float4 myVxColor3 : COLOR2;
	float4 myVxColor4 : COLOR3;
	float2 myUV : TEXCOORD0;
	float2 myUV2 : TEXCOORD1;
	float2 myUV3 : TEXCOORD2;
	float2 myUV4 : TEXCOORD3;
	uint4 myBoneIDs : BONEIDS;
	float4 myBoneWeights : BONEWEIGHTS;
	float3 myTangent : TANGENT;
	float3 myBinormal : BINORMAL;
	float3 myNormal : NORMAL;
};

struct VertexToPixel
{
	float4 myPosition : SV_POSITION;
	float4 myVxColor : COLOR0;
	float4 myVxColor2 : COLOR1;
	float4 myVxColor3 : COLOR2;
	float4 myVxColor4 : COLOR3;
	float2 myUV : TEXCOORD0;
	float2 myUV2 : TEXCOORD1;
	float2 myUV3 : TEXCOORD2;
	float2 myUV4 : TEXCOORD3;
	float3 myTangent : TANGENT;
	float3 myBinormal : BINORMAL;
	float3 myNormal : NORMAL;
};

struct PixelOutput
{
	float4 myColor : SV_TARGET;
};