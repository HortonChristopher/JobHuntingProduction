#include "Sprite.hlsli"

// Shader only for SSAO processing
Texture2D<float4> texNormal : register(t0); //1パス目の法線描画
Texture2D<float> texDepth : register(t1); //1パス目の深度テクスチャ

SamplerState smp : register(s0);

static const int loopCount = 12;

// Required to restore original coordinates
cbuffer sceneBuffer : register(b1)
{
    matrix view; // View
    matrix proj; // Projection
    matrix inverseProjection; // Reverse projection
    float4 normalRandom[loopCount];
};

// Returns a random number based on the current UV value
float random(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898f, 78.233f))) * 43758.5453f);
}

// SSAO (only lightness information for multiplication should be returned)
float PSmain(VSOutput input) : SV_Target
{
    float dp = texDepth.Sample(smp, input.uv); // Current UV depth

    // Restore the original coordinates
    float4 restorePosition = mul(inverseProjection, float4(input.uv * float2(2, -2) + float2(-1, 1), dp, 1));
    restorePosition.xyz = restorePosition.xyz / restorePosition.w;

    float ambientOcclusion = 0.0f;
    float3 norm = normalize((texNormal.Sample(smp, input.uv).xyz * 2) - 1);

    const float radius = 0.05f;

    if (dp < 1.0f)
    {
        for (int i = 0; i < loopCount; ++i)
        {
            float3 omega = normalRandom[i].xyz;

            float dot = dot(norm, omega);
            float sign = sign(dot);
            omega *= sign;
            float4 resPos = mul(proj, mul(view, float4(restorePosition.xyz + omega * radius, 1)));
            resPos.xyz /= resPos.w;
            ambientOcclusion += step(texDepth.Sample(smp, (resPos.xy + float2(1, -1)) * float2(0.5, -0.5)), resPos.z) * dot * sign;
        }

        ambientOcclusion /= (float)loopCount;
    }

    return 1.0f - ambientOcclusion;
}