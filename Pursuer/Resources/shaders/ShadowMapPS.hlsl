#include"ShadowMap.hlsli"

float4 PSmain(VSOutput input) : SV_TARGET
{
	// Sample shadow map using position in light space
	float shadow = shadowMap.SampleCmpLevelZero(shadowSampler, input.positionLS.xy / input.positionLS.w, input.positionLS.z / input.positionLS.w);
	// Compute diffuse color using Lambertian lighting model
	float3 lightDir = normalize(input.positionLS.xyz / input.positionLS.w - cameraPos);
	float diffuse = max(dot(input.normal, lightDir), 0.0f);
	float3 diffuseColor = float3(diffuse, diffuse, diffuse);

	// Compute specular color using Blinn-Phong lighting model
	float3 viewDir = normalize(cameraPos - input.positionLS.xyz / input.positionLS.w);
	float3 halfDir = normalize(lightDir + viewDir);
	float specular = pow(max(dot(input.normal, halfDir), 0.0f), 64.0f);
	float3 specularColor = float3(specular, specular, specular);

	// Apply shadow factor to diffuse and specular colors
	float3 finalColor = (1.0f - shadow) * (diffuseColor + specularColor);

	return float4(finalColor, 1.0f);
}
