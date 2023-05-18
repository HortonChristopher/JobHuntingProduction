cbuffer cbuff0 : register(b0)
{
    matrix viewproj; // View projection matrix
    matrix world; // World matrix
};

// Structure used to communicate from vertex shaders to pixel shaders
struct VSOutput
{
    float4 svpos : SV_POSITION; // Vertex coordinates for system
    float3 normal : NORMAL;
    float2 uv : TEXCOORD; // uv coordinates
};

struct PSOutput
{
    float4 color : SV_TARGET0; // Normal rendering results
    float4 normal : SV_TARGET1; // Normal vector
};
