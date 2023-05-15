#include"DrawShadowOBJ.hlsli"

VSOutput VSmain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    // Apply four scaling rotations to the world matrix to the normals
    float4 wNormal = normalize(mul(world, float4(normal, 0)));
    float4 wPos = mul(world, pos);

    // Value to pass to the pixel shader
    VSOutput output;

    output.svPos = mul(mul(viewProj, world), pos);
    output.shadowPos = mul(mul(lightCameraVP, world), pos);
    output.worldPos = wPos;
    output.normal = wNormal.xyz;
    output.uv = uv;

    return output;
}