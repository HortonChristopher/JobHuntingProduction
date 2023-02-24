#include"ShadowMap.hlsli"

VSOutput VSmain(float4 pos : POSITION)
{
    // Value to pass to pixel shader
    VSOutput output;
    output.svpos = mul(mul(viewproj, world), pos);

    return output;
}