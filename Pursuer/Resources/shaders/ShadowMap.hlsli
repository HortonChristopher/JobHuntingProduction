cbuffer cbuff0 : register(b0)
{
	matrix viewproj; // View Projection Matrix
	matrix world; // World Matrix
	float3 cameraPos; // Camera Position (World View)
};

// Communication structure to communicate from Vertex Shader ot Pixel Shader
struct VSOutput
{
	float4 svpos : SV_POSITION; // System Vertex Coordinates
};

struct VSInput
{
    float4 svpos : POSITION; // System Vertex Coordinates
    float3 normal : NORMAL; // Normal
    float2 uv : TEXCOORD; // UV Cordinates
    uint4 boneIndices : BONEINDICES;
    float4 boneWeights : BONEWEIGHTS;
};