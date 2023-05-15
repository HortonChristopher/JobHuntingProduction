#include "Sprite.hlsli"

Texture2D<float4> tex0 : register(t0); // Texture set in slot 0
Texture2D<float4> tex1 : register(t1); // Texture set in slot 1
Texture2D<float4> tex2 : register(t2); // Texture set in slot 2
Texture2D<float4> tex3 : register(t3); // Texture set in slot 3

SamplerState smp:register(s0);  // Sampler set in slot 0

float4 GetRet(Texture2D<float4> tex, float2 uv)
{
    float w, h, miplevels;

    tex.GetDimensions(0, w, h, miplevels);

    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0, 0, 0, 0);

    // Top row
    ret += tex.Sample(smp, uv + float2(-2 * dx, 2 * dy)) * 1 / 256;
    ret += tex.Sample(smp, uv + float2(-1 * dx, 2 * dy)) * 4 / 256;
    ret += tex.Sample(smp, uv + float2(0 * dx, 2 * dy)) * 6 / 256;
    ret += tex.Sample(smp, uv + float2(1 * dx, 2 * dy)) * 4 / 256;
    ret += tex.Sample(smp, uv + float2(2 * dx, 2 * dy)) * 1 / 256;

    // 2nd row
    ret += tex.Sample(smp, uv + float2(-2 * dx, 1 * dy)) * 4 / 256;
    ret += tex.Sample(smp, uv + float2(-1 * dx, 1 * dy)) * 16 / 256;
    ret += tex.Sample(smp, uv + float2(0 * dx, 1 * dy)) * 24 / 256;
    ret += tex.Sample(smp, uv + float2(1 * dx, 1 * dy)) * 16 / 256;
    ret += tex.Sample(smp, uv + float2(2 * dx, 1 * dy)) * 4 / 256;

    // Center
    ret += tex.Sample(smp, uv + float2(-2 * dx, 0 * dy)) * 6 / 256;
    ret += tex.Sample(smp, uv + float2(-1 * dx, 0 * dy)) * 24 / 256;
    ret += tex.Sample(smp, uv + float2(0 * dx, 0 * dy)) * 36 / 256;
    ret += tex.Sample(smp, uv + float2(1 * dx, 0 * dy)) * 24 / 256;
    ret += tex.Sample(smp, uv + float2(2 * dx, 0 * dy)) * 6 / 256;

    // 4th row
    ret += tex.Sample(smp, uv + float2(-2 * dx, -1 * dy)) * 4 / 256;
    ret += tex.Sample(smp, uv + float2(-1 * dx, -1 * dy)) * 16 / 256;
    ret += tex.Sample(smp, uv + float2(0 * dx, -1 * dy)) * 24 / 256;
    ret += tex.Sample(smp, uv + float2(1 * dx, -1 * dy)) * 16 / 256;
    ret += tex.Sample(smp, uv + float2(2 * dx, -1 * dy)) * 4 / 256;

    // Last row
    ret += tex.Sample(smp, uv + float2(-2 * dx, -2 * dy)) * 1 / 256;
    ret += tex.Sample(smp, uv + float2(-1 * dx, -2 * dy)) * 4 / 256;
    ret += tex.Sample(smp, uv + float2(0 * dx, -2 * dy)) * 6 / 256;
    ret += tex.Sample(smp, uv + float2(1 * dx, -2 * dy)) * 4 / 256;
    ret += tex.Sample(smp, uv + float2(2 * dx, -2 * dy)) * 1 / 256;

    return ret;
}

float4 PSmain(VSOutput input) :SV_TARGET
{
    const float4 texColor0 = tex0.Sample(smp, input.uv) * color;

    float4 returnColor = texColor0;

    float4 bloomAccum = float4(0, 0, 0, 0);

    float2 uvSize = float2(1, 0.5);

    float2 uvOfset = float2(0, 0);

    // Add up the textures in the reduction buffer while shifting them
    for (int i = 0; i < 3; ++i)
    {
        uvOfset.y += uvSize.y;

        uvSize *= 0.5f;

        bloomAccum += GetRet(tex3, input.uv * uvSize + uvOfset);
    }

    // Reflects high brightness
    returnColor += tex2.Sample(smp, input.uv) + saturate(bloomAccum);

    return returnColor;
}
