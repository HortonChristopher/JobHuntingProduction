#include"LightStruct.hlsli"

cbuffer cbuff0 : register(b0)
{
	matrix viewproj; // View projection matrix
	matrix world; // World matrix
	float3 cameraPos; // Camera coordinates (world coordinates)
	float4 color;
	matrix lightViewProj;
};

cbuffer cbuff1 : register(b1)
{
	float3 m_ambient  : packoffset(c0); // Ambient coefficient
	float3 m_diffuse  : packoffset(c1); // Diffuse coefficient
	float3 m_specular : packoffset(c2); // Specular coefficient
	float m_alpha : packoffset(c2.w);	// Alpha
}

cbuffer cbuff2 : register(b2)
{
	float3 ambientColor;
	DirLight dirLights[DIRLIGHT_NUM];
	PointLight pointLights[POINTLIGHT_NUM];
	SpotLight spotLights[SPOTLIGHT_NUM];
	CircleShadow circleShadows[CIRCLESHADOW_NUM];
}

// Structure used to communicate from vertex shaders to pixel shaders
struct VSOutput
{
	float4 svpos : SV_POSITION; // Vertex coordinates for system
	float4 shadowPos : SHADOWPOS;
	float4 worldpos : POSITION; // World coordinate
	float3 normal :NORMAL; // Normal vector
	float2 uv  :TEXCOORD; // uv coordinate
};