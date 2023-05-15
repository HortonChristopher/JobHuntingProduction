#include "Particle.hlsli"

struct VSInput
{
    float4 position : POSITION;
    uint pad1 : TEXCOORD1a;

    float3 rotation : ROTATION;
    // Billboard usage check
    uint billboardActive : BILLBOARD;

    float4 color : COLOR;
    float2 scale : SCALE;

    float2 pad0 : TEXCOORD4a;
    float3 pad3 : TEXCOORD0a;
    // Death flag
    uint pad2 : TEXCOORD2a;
};

VSOutput VSmain(VSInput input)
{
    VSOutput output; // Value to pass to pixel shaders

    output.pos = float4(input.position);
    output.scale = input.scale;
    output.color = input.color;
    output.rotation = input.rotation;
    output.billboardActive = input.billboardActive;

    return output;
}