#include "SkyBox.hlsli"

float3 skyColor(float2 uv)
{
    float atmosphere = sqrt(1.0 - uv.y);
    float3 skyRGB = float3(0.2, 0.4, 0.8);
    float scatter = pow(0.8f, 1.0 / 15.0);

    scatter = 1.0 - clamp(scatter, 0.8, 1.0);

    float3 scatterColor = lerp(float3(1, 1, 1), float3(1.0, 0.3, 0.0) * 1.5, scatter);

    return lerp(skyRGB, float3(scatterColor), atmosphere / 1.3);
}

float3 sunColor(float2 uv)
{
    float sun = 1.0 - distance(uv, float2(0.54f, 0.8f));
    sun = clamp(sun, 0.0, 1.0);

    float glow = sun;
    glow = clamp(glow, 0.0, 1.0);

    sun = pow(sun, 1000);
    sun *= 100.0;
    sun = clamp(sun, 0.0, 1.0);

    glow = pow(glow, 60) * 1.0;
    glow = pow(glow, (uv.y));
    glow = clamp(glow, 0.0, 1.0);

    sun *= pow(dot(uv.y, uv.y), 1.0 / 1.65);
    glow *= pow(dot(uv.y, uv.y), 1.0 / 2.0);

    sun += glow;

    float3 sunRGB = float3(1.0, 0.6, 0.05) * sun;

    return float3(sunRGB);
}

PSOutput PSmain(VSOutput input)
{
    PSOutput output;

    float2 uv = input.uv;
    float3 sky = skyColor(uv);
    float3 sun = sunColor(uv);

    output.normal.rgb = float3((input.normal.xyz + 1.0f) / 2.0f);
    output.normal.a = 1;
    output.color = float4(sky + sun, 1.0);

    return output;
}