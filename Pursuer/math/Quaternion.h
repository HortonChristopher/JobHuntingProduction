#pragma once

#include "Vector.h"
#include "Matrix.h"

#include<cmath>

struct Quaternion
{
	float x; // X component
	float y; // Y component
	float z; // Z component
	float w; // W component
};

// Create quaternion
Quaternion quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);
Quaternion quaternion(const Vector3& v, float angle);
Quaternion quaternion(const Vector3& v, const Quaternion& q);

// Find the inner product
float dot(const Quaternion& q1, const Quaternion& q2);

// Find the norm
float length(const Quaternion& q);

// Normalize
Quaternion normalize(const Quaternion& q);

// Find conjugate of four-dimensional numbers
Quaternion conjugate(const Quaternion& q);

// Unary operator overloading
Quaternion operator + (const Quaternion& q);
Quaternion operator - (const Quaternion& q);

// Assignment operator overloading
Quaternion& operator += (Quaternion& q1, const Quaternion& q2);
Quaternion& operator -= (Quaternion& q1, const Quaternion& q2);
Quaternion& operator *= (Quaternion& q, float s);
Quaternion& operator /= (Quaternion& q, float s);
Quaternion& operator *= (Quaternion& q1, const Quaternion& q2);

// Binary operator overloading
Quaternion operator + (const Quaternion& q1, const Quaternion& q2);
Quaternion operator - (const Quaternion& q1, const Quaternion& q2);
Quaternion operator * (const Quaternion& q1, const Quaternion& q2);
Quaternion operator * (const Quaternion& q, float s);
Quaternion operator * (float s, const Quaternion& q);
Quaternion operator / (const Quaternion& q1, float s);

Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t);
Quaternion lerp(const Quaternion& q1, const Quaternion& q2, float t);
Matrix rotate(const Quaternion& q);
Quaternion quaternion(const Matrix& m);
Vector3 getAxis(const Quaternion& q);