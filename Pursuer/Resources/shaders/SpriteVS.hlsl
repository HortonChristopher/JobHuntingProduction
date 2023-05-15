#include "Sprite.hlsli"

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput output; // Value to pass to the pixel shader

	output.svpos = mul(mat, pos);
	output.uv = uv;

	return output;
}