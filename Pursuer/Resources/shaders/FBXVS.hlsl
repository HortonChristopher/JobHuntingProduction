#include "FBX.hlsli"

struct SkinOutput
{
    float4 pos;
    float3 normal;
};

// Skinning Calculation
SkinOutput ComputeSkin(VSInput input)
{
    // Zero clear
    SkinOutput output = (SkinOutput)0;

    uint iBone; // Bone number to be calculated
    float weight; // Bone weight
    matrix m; // Skinning weight

    iBone = input.boneIndices.x;
    weight = input.boneWeights.x;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.svpos);
    output.normal += weight * mul((float3x3) m, input.normal);

    iBone = input.boneIndices.y;
    weight = input.boneWeights.y;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.svpos);
    output.normal += weight * mul((float3x3) m, input.normal);

    iBone = input.boneIndices.z;
    weight = input.boneWeights.z;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.svpos);
    output.normal += weight * mul((float3x3) m, input.normal);

    iBone = input.boneIndices.w;
    weight = input.boneWeights.w;
    m = matSkinning[iBone];
    output.pos += weight * mul(m, input.svpos);
    output.normal += weight * mul((float3x3) m, input.normal);

    return output;
}

VSOutput VSmain(VSInput input)
{
    SkinOutput skinned = ComputeSkin(input);

    // Apply four scaling rotations to the world matrix to the normals
    float4 wnormal = normalize(mul(world, float4(skinned.normal, 0)));

    // Value to pass to the pixel shader	
    VSOutput output;

    output.svpos = mul(mul(viewproj, world), skinned.pos);
    output.worldpos = mul(world, skinned.pos);
    output.normal = wnormal.xyz;
    output.uv = input.uv;
    output.shadowPos = mul(mul(lightCameraVP, world), skinned.pos);

    return output;
}