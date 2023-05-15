cbuffer cbuff0 : register(b0)
{
	matrix mat; // View projection matrix
	matrix matBillboard; // Billboard matrix
};

// Structure used to communicate from vertex shaders to pixel shaders
struct VSOutput
{
	float4 pos : POSITION; // Position coordinates
	float2 scale : TEXCOORD; // Scale
	float4 color : COLOR;
	float3 rotation : ROTATION; // Rotation
	uint billboardActive : BILLBOARD;
};

struct GSOutput
{
	float4 svpos : SV_POSITION; // Vertex coordinates for system
	float2 uv : TEXCOORD; // uv coordinates
	float4 color : COLOR;
};
