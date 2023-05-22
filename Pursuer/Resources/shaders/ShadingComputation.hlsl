struct InputData
{
	// Acceleration
    float3 accel;
    float s_alpha;

	// Scale
    float2 scaleAce;
    float2 s_scale ;
    float2 e_scale ;
    float e_alpha ;

	// End frame
    int num_frame;

    float3 s_color;
    float pad;
    float3 s_rotation;
    float pad2;
	// Final value
    float3 e_color;
    float pad3;
    float3 e_rotation;
    float pad4;
};

struct OutputData
{
    float3 position;
	// Death flag
    uint isDead;

    float3 rotation;
	// Billboard or not
    uint billboardActive;

    float4 color;
    float2 scale;
    float2 scaleVel;

    float3 velocity;
    uint frame;
};

// Input data
StructuredBuffer<InputData> g_inputData : register(t0);
StructuredBuffer<OutputData> g_inputData2 : register(t1);

// Output (data)
RWStructuredBuffer<OutputData> g_outputData : register(u0);

//[RootSignature(RS)]
[numthreads(1, 1, 1)]

void CSmain(uint3 gID : SV_DispatchThreadID)
{
    InputData input = g_inputData[gID.x];
    OutputData input2 = g_inputData2[gID.x];
    float3 velocity = input2.velocity;
	// Converts progression to a range of 0 to 1
    const float f = (float) input.num_frame / input2.frame;

	// Add acceleration to velocity
    velocity += input.accel;

	// Movement by speed
    input2.position += velocity;
    input2.velocity = velocity;

	// Linear interpolation for color			
    // Linear interpolation of alpha values
    input2.color = float4(input.s_color + (input.e_color - input.s_color) / f, input.s_alpha + (input.e_alpha - input.s_alpha) / f);

	// Scale velocity plus acceleration
    input2.scaleVel += input.scaleAce;

    // Add speed to scale
    input2.scale += input2.scaleVel;

    // Linear interpolation of rotation
    input2.rotation = float4(input.s_rotation + (input.e_rotation - input.s_rotation) / f, 0);
    input2.isDead = step(input.num_frame, input2.frame);

    // Count elapsed frames
    input2.frame++;
	
	// Output (data)
    OutputData output = input2;
    g_outputData[gID.x] = output;
}