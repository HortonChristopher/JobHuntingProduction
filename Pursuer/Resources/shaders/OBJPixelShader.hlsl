#include "OBJShaderHeader.hlsli"

Texture2D<float4> tex0 : register(t0);  // Texture set in slot 0
Texture2D<float4> tex1 : register(t1); // Texture set in slot 0

SamplerState smp0 : register(s0); // Sampler set in slot 0
SamplerComparisonState smp1 : register(s1); // Sampler set in slot 1

float4 PSmain(VSOutput input) : SV_TARGET
{
    // Texture mapping
    float4 texcolor = tex0.Sample(smp0, input.uv);

    const float zlnLVP = input.shadowPos.z / input.shadowPos.w;

    float2 shadowMapUV;
    shadowMapUV.x = (1.0f + input.shadowPos.x / input.shadowPos.w) * 0.5f;
    shadowMapUV.y = (1.0f - input.shadowPos.y / input.shadowPos.w) * 0.5f;
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;

    float shadow = tex1.SampleCmpLevelZero(smp1, shadowMapUV, zlnLVP);

    float3 shadowColor = texcolor.xyz * 0.5f;

    texcolor.xyz = lerp(texcolor.xyz, shadowColor, shadow);

    // Color by shading
    // Gloss
    const float shininess = 4.0f;
    // Direction vector from vertex to viewpoint
    float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

    // Ambient reflected light
    float3 ambient = 0.26;

    // Color by shading
    float4 shadecolor = float4(ambientColor * ambient, m_alpha);

    // Directional light source
    for (int i = 0; i < DIRLIGHT_NUM; i++)
    {
        // Inner product of the vector toward the light and the normal
        float3 dotlightnormal = dot(dirLights[i].lightv, input.normal);

        // Reflected light vector
        float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal);

        // Diffuse reflection light
        float3 diffuse = dotlightnormal * 0.80;
        float3 halfVec = normalize(dirLights[i].lightv + eyedir);

        // Specular reflection
        float3 specular = pow(saturate(dot(reflect, halfVec)), shininess) * 0.10;

        // All add up
        shadecolor.rgb += (diffuse + specular) * dirLights[i].lightcolor;
    }

    // Positional light source
    for (int i = 0; i < POINTLIGHT_NUM; i++)
    {
        // Light Vector
        float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;

        // Vector Length
        float d = length(lightv);

        // Normalize to unit vector
        lightv = normalize(lightv);

        // Distance attenuation coefficient
        float atten = 1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d + pointLights[i].lightatten.z * d * d);

        // Inner product of vectors toward the light
        float3 dotlightnormal = dot(lightv, input.normal);

        // Reflected light vector
        float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);

        // Diffuse reflection light
        float3 diffuse = dotlightnormal * m_diffuse;
        float3 halfVec = normalize(lightv + eyedir);

        // Specular reflection
        float3 specular = pow(saturate(dot(reflect, halfVec)), shininess) * m_specular;

        // All add up
        shadecolor.rgb += atten * (diffuse + specular) * pointLights[i].lightcolor * pointLights[i].active;
    }

    // Spotlight
    for (int i = 0; i < SPOTLIGHT_NUM; i++)
    {
        // Light Vector
        float3 lightv = spotLights[i].lightpos - input.worldpos.xyz;

        // Vector Length
        float d = length(lightv);

        // Normalize to unit vector
        lightv = normalize(lightv);

        // Distance attenuation coefficient
        float atten = saturate(1.0f / (spotLights[i].lightatten.x + spotLights[i].lightatten.y * d + spotLights[i].lightatten.z * d * d));

        // Angular attenuation
        float cos = dot(lightv, spotLights[i].lightv);

        // Damping from the start angle to the end angle of damping
        //1x luminance inside the attenuation start angle; 0x luminance outside the attenuation end angle
        float angleatten = smoothstep(spotLights[i].lightfactoranglecos.y, spotLights[i].lightfactoranglecos.x, cos);

        // Multiply by angle attenuation
        atten *= angleatten;

        // Inner product of vectors toward the light
        float3 dotlightnormal = dot(lightv, input.normal);

        // Reflected light vector
        float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);

        // Diffuse reflection light
        float3 diffuse = dotlightnormal * m_diffuse;

        // Specular reflection
        float3 halfVec = normalize(lightv + eyedir);
        float3 specular = pow(saturate(dot(reflect, halfVec)), shininess) * m_specular;

        // All add up
        shadecolor.rgb += atten * (diffuse + specular) * spotLights[i].lightcolor * clamp(spotLights[i].active, 0, 1.0f);
    }

    // Circle shadow
    for (int i = 0; i < CIRCLESHADOW_NUM; i++)
    {
        // Vector from object surface to caster
        float3 casterv = circleShadows[i].casterPos - input.worldpos.xyz;

        // Distance in projection direction
        float d = dot(casterv, circleShadows[i].dir);

        // Distance attenuation coefficient
        float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y * d + circleShadows[i].atten.z * d * d));

        // If the distance is negative, it will be zero.
        atten *= step(0, d);

        // Coordinates of virtual light
        float3 lightpos = circleShadows[i].casterPos + circleShadows[i].dir * circleShadows[i].distanceCasterLight;

        // Vector from object surface to light (unit vector)
        float3 lightv = normalize(lightpos - input.worldpos.xyz);

        // Angular attenuation
        float cos = dot(lightv, circleShadows[i].dir);

        // Damping from the start angle to the end angle of damping
        // 1x luminance inside the attenuation start angle; 0x luminance outside the attenuation end angle
        float angleatten = smoothstep(circleShadows[i].factorAngleCos.y, circleShadows[i].factorAngleCos.x, cos);

        // Multiply by angle attenuation
        atten *= angleatten;

        // Reduce all
        shadecolor.rgb -= atten * clamp(circleShadows[i].active, 0, 1.0f);
    }

    // Drawing by color with shading
    float4 returnColor = shadecolor * texcolor * color;

    return pow(returnColor, 1.0 / 2.2);
}