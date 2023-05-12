#include"ShadowMap.hlsli"

struct VSOutput
{
	float4 svpos : SV_POSITION; // System Vertex Coordinates
	float4 positionLS : TEXCOORD0; // Vertex position in light space
};

VSOutput VSmain(VSInput input)
{
	// Compute vertex position in world space
	float4 positionWS = mul(world, input.svpos);
	// Compute vertex position in light space
	float4 positionLS = mul(lightViewProj, positionWS);

	// Value to pass to pixel shader
	VSOutput output;
	output.svpos = mul(mul(viewproj, world), input.svpos);
	output.positionLS = positionLS;

	return output;
}