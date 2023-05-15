#include "Sprite.hlsli"

Texture2D<float4> tex0 : register(t0); // Texture set in slot 0
SamplerState smp : register(s0); // Sampler set in slot 0

float4 PSmain(VSOutput input) :SV_TARGET
{
    float4 colorSRGB = tex0.Sample(smp, input.uv);
    float4 colorLINEAR = pow(colorSRGB, 2.2);
    float4 returnColor = colorLINEAR * color;

    returnColor = pow(returnColor, 1.0 / 2.2);

    return returnColor;
}