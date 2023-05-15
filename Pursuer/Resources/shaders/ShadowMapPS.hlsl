#include"ShadowMap.hlsli"

float4 PSmain(VSOutput input) : SV_TARGET
{
	return float4(input.svpos.z, input.svpos.z, input.svpos.z, 1.0f);
}