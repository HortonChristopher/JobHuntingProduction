#include"ShadowMap.hlsli"

static const int MAX_BONES = 256;

cbuffer skinning : register(b1)
{
    matrix matSkinning[MAX_BONES];
}

// Skinning Calculation
float4 ComputeSkin(VSInput input)
{
    // Zero clear
    float4 output = float4(0, 0, 0, 0);

    uint iBone; // Bone number to be calculated
    float weight; // Bone weight
    matrix m; // Skinning Weight

    iBone = input.boneIndices.x;
    weight = input.boneWeights.x;
    m = matSkinning[iBone];
    output += weight * mul(m, input.svpos);

    iBone = input.boneIndices.y;
    weight = input.boneWeights.y;
    m = matSkinning[iBone];
    output += weight * mul(m, input.svpos);

    iBone = input.boneIndices.z;
    weight = input.boneWeights.z;
    m = matSkinning[iBone];
    output += weight * mul(m, input.svpos);

    iBone = input.boneIndices.w;
    weight = input.boneWeights.w;
    m = matSkinning[iBone];
    output += weight * mul(m, input.svpos);

    return output;
}

VSOutput VSmain(VSInput input)
{
    float4 skinnedPos = ComputeSkin(input);

    // Value to pass to the pixel shader
    VSOutput output;
    output.svpos = mul(mul(viewproj, world), skinnedPos);

    return output;
}