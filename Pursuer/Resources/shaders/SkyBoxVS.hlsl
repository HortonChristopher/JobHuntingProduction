#include "SkyBox.hlsli"

VSOutput VSmain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    // Value to pass to the pixel shader	
    VSOutput output;

    output.svpos = mul(mul(viewproj, world), pos);

    float4 wnormal = normalize(mul(world, float4(normal, 0)));

    output.normal = wnormal.xyz;
    output.uv = uv;

    return output;
}
