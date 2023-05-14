#include "FBX.hlsli"

Texture2D<float4> tex0 : register(t0); // Texture set in slot 0
Texture2D<float> tex1 : register(t1); // Texture set in slot 1

SamplerState smp0 : register(s0); // Sampler set in slot 0
SamplerComparisonState smp1 : register(s1); // Sampler set in slot 1

PSOutput PSmain(VSOutput input) : SV_TARGET
{
    PSOutput output;

    // Texture mapping
    float4 colorSRGB = tex0.Sample(smp0, input.uv);
    float4 texColor = pow(colorSRGB, 2.2);

    const float zlnLVP = input.shadowPos.z / input.shadowPos.w;

    float2 shadowMapUV = input.shadowPos.xy / input.shadowPos.w;
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;

    float shadow = tex1.SampleCmpLevelZero(smp1, shadowMapUV, zlnLVP - 0.0001f);
    float3 shadowColor = texColor.xyz * 0.3f;

    texColor.xyz = lerp(shadowColor, texColor.xyz, shadow);

    // Gloss
    const float shininess = 4.0f;

    //  Direction vector from vertex to viewpoint
    float3 eyeDir = normalize(cameraPos - input.worldpos.xyz);

    // Ambient reflected light
    float3 ambient = 0.76f;
    float3 m_diffuse = float3(0.5f, 0.5f, 0.5f);
    float3 m_specular = float3(0.1f, 0.1f, 0.1f);
    float m_alpha = 1;

    // Color by shading
    float4 shadeColor = float4(ambientColor * ambient, m_alpha);

    // Directional light source
    for (int i = 0; i < DIRLIGHT_NUM; i++)
    {
        // Inner product of the vector toward the light and the normal
        float3 dotLightNormal = dot(dirLights[i].lightV, input.normal);

        // Reflected light vector
        float3 reflect = normalize(-dirLights[i].lightV + 2 * dotLightNormal * input.normal);

        // Diffuse reflection light
        float3 diffuse = dotLightNormal * 0.80;
        float3 halfVec = normalize(dirLights[i].lightV + eyeDir);

        // Mirror reflection light
        float3 specular = pow(saturate(dot(reflect, halfVec)), shininess) * 0.10;

        // All add up
        shadeColor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
    }

    // Positional light source
    for (int i = 0; i < POINTLIGHT_NUM; i++)
    {
        // Light Vector
        float3 lightV = pointLights[i].lightpos - input.worldpos.xyz;

        // Vector Length
        float d = length(lightV);

        // Normalize to unit vector
        lightV = normalize(lightV);

        // Distance attenuation coefficient
        float atten = saturate(1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d + pointLights[i].lightatten.z * d * d));

        // Inner product of vectors toward the light
        float3 dotLightNormal = dot(lightV, input.normal);

        // Reflected light vector
        float3 reflect = normalize(-lightV + 2 * dotLightNormal * input.normal);

        // Diffuse reflection light
        float3 diffuse = dotLightNormal * m_diffuse;
        float3 halfVec = normalize(lightV + eyeDir);

        // Mirror Reflected Light
        float3 specular = pow(saturate(dot(reflect, halfVec)), shininess) * m_specular;

        // All add up
        shadeColor.rgb += atten * (diffuse + specular) * pointLights[i].lightcolor * pointLights[i].active;
    }

    // Spotlight
    for (int i = 0; i < SPOTLIGHT_NUM; i++)
    {
        // Light Vector
        float3 lightV = spotLights[i].lightpos - input.worldpos.xyz;

        // Vector Length
        float d = length(lightV);

        // Normalize to unit vector
        lightV = normalize(lightV);

        // Distance attenuation coefficient
        float atten = saturate(1.0f / (spotLights[i].lightatten.x + spotLights[i].lightatten.y * d + spotLights[i].lightatten.z * d * d));

        // Angular attenuation
        float cos = dot(lightV, spotLights[i].lightV);

        // Damping from the start angle to the end angle of damping
        // 1x luminance inside the attenuation start angle; 0x luminance outside the attenuation end angle
        float angleAtten = smoothstep(spotLights[i].lightfactoranglecos.y, spotLights[i].lightfactoranglecos.x, cos);

        // Multiply by angle attenuation
        atten *= angleAtten;

        // Inner product of vectors toward the light
        float3 dotLightNormal = dot(lightV, input.normal);

        // Reflected light vector
        float3 reflect = normalize(-lightV + 2 * dotLightNormal * input.normal);

        // Diffuse reflection light
        float3 diffuse = dotLightNormal * m_diffuse;

        // Mirror Reflected Light
        float3 halfVec = normalize(lightV + eyeDir);
        float3 specular = pow(saturate(dot(reflect, halfVec)), shininess) * m_specular;

        // All add up
        shadeColor.rgb += atten * (diffuse + specular) * spotLights[i].lightcolor * clamp(spotLights[i].active, 0, 1.0f);
    }

    // Circle Shadow
    for (int i = 0; i < CIRCLESHADOW_NUM; i++)
    {
        // Vector from object surface to caster
        float3 casterV = circleShadows[i].casterPos - input.worldpos.xyz;

        // Distance in projection direction
        float d = dot(casterV, circleShadows[i].dir);

        // Distance attenuation coefficient
        float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y * d + circleShadows[i].atten.z * d * d));

        // If the distance is negative, it will be zero.
        atten *= step(0, d);

        // Coordinates of virtual light
        float3 lightpos = circleShadows[i].casterPos + circleShadows[i].dir * circleShadows[i].distanceCasterLight;

        // Vector from object surface to light (unit vector)
        float3 lightV = normalize(lightpos - input.worldpos.xyz);

        // Angular attenuation
        float cos = dot(lightV, circleShadows[i].dir);

        // Damping from the start angle to the end angle of damping
        // 1x luminance inside the attenuation start angle 0x luminance outside the attenuation end angle
        float angleAtten = smoothstep(circleShadows[i].factorAngleCos.y, circleShadows[i].factorAngleCos.x, cos);

        // Multiply by angle attenuation
        atten *= angleAtten;

        // Reduce all
        shadeColor.rgb -= atten * clamp(circleShadows[i].active, 0, 1.0f);
    }

    // Drawing by color with shading
    float4 returnColor = shadeColor * texColor * color;

    output.color = pow(returnColor, 1.0 / 2.2);
    output.normal.rgb = float3((input.normal.xyz + 1.0f) / 2.0f);
    output.normal.a = 1;
    output.depth = float4(input.svpos.z, input.svpos.z, input.svpos.z, 1.0f);

    return output;
}