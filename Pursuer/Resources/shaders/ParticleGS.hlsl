#include "Particle.hlsli"

// Number of vertices of quadrilateral
static const uint vnum = 4;

// Offset table from center
static const float4 offset_array[vnum] =
{
	float4(-0.5f, -0.5f, 0, 0),	// 左下
	float4(-0.5f, +0.5f, 0, 0),	// 左上
	float4(+0.5f, -0.5f, 0, 0),	// 右下
	float4(+0.5f, +0.5f, 0, 0)	// 右上
};

// UV table (top left 0,0 bottom right 1,1)
static const float2 uv_array[vnum] =
{
	float2(0, 1),	// Lower left
	float2(0, 0),	// Upper left
	float2(1, 1),	// Lower right
	float2(1, 0) 	// Upper right
};

static const matrix MatInitialize = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

// Outputs a rectangle from a point input
[maxvertexcount(vnum)]
void GSmain(point VSOutput input[1] : SV_POSITION, inout TriangleStream<GSOutput> output)
{
	GSOutput element;
	element.color = input[0].color;

	float4 offset = float4(0, 0, 0, 0);

	for (uint i = 0; i < vnum; i++)
	{
		// Scaling offset from center
		offset = offset_array[i];
		offset.x *= input[0].scale.x;
		offset.y *= input[0].scale.y;

		const matrix rotationX =
		{
			1, 0, 0, 0,
			0, cos(input[0].rotation.x), -sin(input[0].rotation.x), 0,
			0, sin(input[0].rotation.x), cos(input[0].rotation.x), 0,
			0, 0, 0, 1
		};

		const matrix rotationY =
		{
			cos(input[0].rotation.y), 0, sin(input[0].rotation.y), 0,
			0, 1, 0, 0,
			-sin(input[0].rotation.y), 0, cos(input[0].rotation.y), 0,
			0, 0, 0, 1
		};

		const matrix rotationZ =
		{
			cos(input[0].rotation.z), -sin(input[0].rotation.z), 0, 0,
			sin(input[0].rotation.z), cos(input[0].rotation.z), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		matrix rotation = MatInitialize;

		rotation = mul(rotationZ, rotation);
		rotation = mul(rotationX, rotation);
		rotation = mul(rotationY, rotation);

		// If input[0].billboardActive == 1, apply the billboard matrix; if 0, apply the rotation matrix calculated above.
		offset = mul(lerp(rotation, matBillboard, step(0.5, input[0].billboardActive)), offset);

		// Shift by offset (world coordinates)
		element.svpos = input[0].pos + offset;

		// View projection transformation
		element.svpos = mul(mat, element.svpos);
		element.uv = uv_array[i];

		output.Append(element);
	}
}