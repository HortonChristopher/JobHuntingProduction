#include "OBJShaderHeader.hlsli"

VSOutput VSmain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    // Apply four scaling rotations to the world matrix to the normals
    float4 wnormal = normalize(mul(world, float4(normal, 0)));
    float4 wpos = mul(world, pos);

    // Value to pass to the pixel shader	
    VSOutput output;

    output.svpos = mul(mul(viewproj, world), pos);
    output.shadowPos = mul(mul(lightViewProj, world), pos);
    output.worldpos = wpos;
    output.normal = wnormal.xyz;
    output.uv = uv;

    return output;
}