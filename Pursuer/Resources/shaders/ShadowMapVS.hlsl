#include"ShadowMap.hlsli"

VSOutput VSmain(VSInput input)
{
    // Value to pass to the pixel shader
    VSOutput output;

    output.svpos = mul(mul(viewproj, world), pos);

    return output;
}