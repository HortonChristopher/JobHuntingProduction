#include "Particle.hlsli"

Texture2D<float4> tex0 : register(t0); // Texture set in slot 0
SamplerState smp : register(s0); // Sampler set in slot 0

float4 PSmain(GSOutput input) : SV_TARGET
{
    float4 texColor = tex0.Sample(smp, input.uv);

    texColor = float4(texColor.xyz * texColor.w, 1);

    return texColor * float4(input.color.rgb * input.color.a, 1);
}