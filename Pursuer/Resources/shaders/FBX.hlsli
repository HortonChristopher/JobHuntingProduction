#include "LightStruct.hlsli"

cbuffer cbuff0 : register(b0)
{
	matrix viewproj; // View projection matrix
	matrix world; // World matrix
	float3 cameraPos; // Camera coordinates (world coordinates)
	float4 color;
}

cbuffer cbuff1 : register(b1)
{
	matrix lightCameraVP; // View projection matrix
	float3 lightCameraPos; // Camera coordinates (world coordinates)
}

cbuffer cbuff2 : register(b2)
{
	float3 ambientColor;
	DirLight dirLights[DIRLIGHT_NUM];
	PointLight pointLights[POINTLIGHT_NUM];
	SpotLight spotLights[SPOTLIGHT_NUM];
	CircleShadow circleShadows[CIRCLESHADOW_NUM];
}

// Bone max
static const int MAX_BONES = 256;
cbuffer skinning : register(b3)
{
	matrix matSkinning[MAX_BONES];
}

// Used for communication from the point shader to the pixel shader
struct VSOutput
{
	float4 svpos : SV_POSITION; // Vertex coordinates for system
	float4 shadowPos : TEXCOORD1; // Shadow coordinate
	float4 worldpos : POS; // World coordinate
	float3 normal : NORMAL; // Normal
	float2 uv : TEXCOORD; // UV
};

// Vertex Buffer Entry
struct VSInput
{
	float4 svpos : POSITION; // Vertex coordinates for system
	float3 normal : NORMAL; // Normal vector
	float2 uv : TEXCOORD; // uv value
	uint4 boneIndices : BONEINDICES; // Bone number
	float4 boneWeights : BONEWEIGHTS; // Bone weight
};

struct PSOutput
{
	float4 color : SV_TARGET0; // Normal rendering results
	float4 normal : SV_TARGET1; // Normal vector
	float4 depth : SV_TARGET2; // Depth value
};