cbuffer cbuff0 : register(b0)
{
	matrix viewproj; // View Projection Matrix
	matrix world; // World Matrix
	float3 cameraPos; // Camera Position (World View)
	matrix lightViewProj; // Light View Projection Matrix
};

// Communication structure to communicate from Vertex Shader to Pixel Shader
struct VSOutput
{
	float4 svpos : SV_POSITION; // System Vertex Coordinates
	float4 positionLS : POSITION; // Position in Light Space
};

struct VSInput
{
	float4 svpos : POSITION; // System Vertex Coordinates
	float3 normal : NORMAL; // Normal
	float2 uv : TEXCOORD; // UV Cordinates
	uint4 boneIndices : BONEINDICES;
	float4 boneWeights : BONEWEIGHTS;
};

Texture2D shadowMap : register(t0);
SamplerComparisonState shadowSampler : register(s0);

// Vertex Shader
VSOutput VS(VSInput input)
{
	VSOutput output;
	// Compute world position of vertex
	float4 worldPosition = mul(input.svpos, world);

	// Compute position of vertex in light space
	output.positionLS = mul(worldPosition, lightViewProj);

	// Compute position of vertex in clip space
	output.svpos = mul(worldPosition, viewproj);

	// Pass other attributes through to pixel shader
	output.normal = input.normal;
	output.uv = input.uv;
	output.boneIndices = input.boneIndices;
	output.boneWeights = input.boneWeights;

	return output;
}

// Pixel Shader
float4 PS(VSOutput input) : SV_TARGET
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