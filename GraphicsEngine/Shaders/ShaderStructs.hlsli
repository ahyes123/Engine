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

cbuffer FrameBuffer : register(b0)
{
	float4x4 FB_ToView;
	float4x4 FB_ToProjection;
	float3 FB_CamTranslation;
	uint FB_RenderMode;
}

cbuffer ObjectBuffer : register(b1)
{
	float4x4 OB_ToWorld;
	bool OB_HasBones;
	float3 OB_padding;
	float4x4 OB_BoneData[128];
}

cbuffer MaterialBuffer : register(b2)
{
	float3 MB_Albedo;
	float MB_padding;
}

struct VertexToPixel
{
	float4 myPosition : SV_POSITION;
	float4 myVxPosition : POSITION;
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

struct GBufferOutput
{
	float4 Albedo : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Material : SV_TARGET2;
	float4 VertexNormal : SV_TARGET3;
	float4 WorldPosition : SV_TARGET4;
	float AmbientOcclusion : SV_TARGET5;
};

struct DeferredVertexInput
{
	unsigned int myIndex : SV_VertexID;
};

struct DeferredVertexToPixel
{
	float4 position : SV_POSITION;
	float2 UV : TEXCOORD;
};

struct DeferredPixelOutput
{
	float4 Color : SV_TARGET;
};