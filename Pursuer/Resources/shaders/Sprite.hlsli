cbuffer cbuff0 : register(b0)
{
	float4 color; // Color (RGBA)
	matrix mat; // 3D transformation matrix
};

// Structure used to communicate from vertex shaders to pixel shaders
struct VSOutput
{
	float4 svpos : SV_POSITION; // Vertex coordinates for system
	float2 uv  :TEXCOORD; // uv coordinates
};