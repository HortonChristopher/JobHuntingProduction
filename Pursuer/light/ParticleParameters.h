#pragma once

// intsafe.h is a header file provided by the Windows SDK (Software Development Kit) that contains functions and macros for performing safe integer operations.
// It provides utilities for handling integer arithmetic operations and ensures that the operations do not result in integer overflow or underflow.
// By including intsafe.h, you gain access to functions and macros such as SafeAdd, SafeMultiply, SafeSubtract, etc., which can be used to perform arithmetic operations on integers in a safe and reliable manner.
#include <intsafe.h>
#include <string>

#include "Vector.h"

struct ParticleParameters
{
	// Acceleration
	Vector3 accel = { 0,0,0 };
	float s_alpha = 1;

	// Scale
	Vector2 scaleAce = { 0,0 };
	Vector2 s_scale = { 0,0 };
	Vector2 e_scale = { 0,0 };
	float e_alpha = 1;

	// End frame
	int num_frame = 60;

	Vector3 s_color = { 0,0,0 };
	float pad;
	Vector3 s_rotation = { 0,0,0 };
	float pad2;
	// Final value
	Vector3 e_color = { 1,1,1 };
	float pad3;
	Vector3 e_rotation = { 0,0,0 };
	float pad4;
};

struct OutputData
{
	Vector3 position;
	// Death Flag
	UINT isDead;

	Vector3 rotation;
	// Billboard or not
	UINT billboardActive;

	DirectX::XMFLOAT4 color;
	Vector2 scale;
	Vector2 scaleVel;

	Vector3 velocity;
	UINT frame;
};