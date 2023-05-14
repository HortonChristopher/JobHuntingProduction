#pragma once

#include <array>
#include <DirectXMath.h>
#include <cmath>
#include <cstring>

#include "Vector.h"

class Matrix
{
public:
	std::array<std::array<float, 4>, 4> m =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1,
	};

	Matrix();
	Matrix(const float val);
	Matrix(const DirectX::XMMATRIX& mat);
	Matrix(const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23,
		const float m30, const float m31, const float m32, const float m33);

	DirectX::XMMATRIX convertToXMMATRIX();

	// Matrix(const float val[4][4]);
	// Find the monomial matrix
	static Matrix identity();

	static DirectX::XMMATRIX convertToXMMATRIX(const Matrix& mat);

	// Scaling Settings
	static Matrix scale(const Vector3& s);

	// Setting the rotation matrix
	static Matrix rotateX(float angle);
	static Matrix rotateY(float angle);
	static Matrix rotateZ(float angle);

	// Parallel shift matrix creation
	static Matrix translate(const Vector3& t);

	// Coordinate transformation (multiply vectors and matrices)
	static Vector3 transform(const Vector3& v, const Matrix& m);
};

// Binary operator overloading
const Matrix operator* (const Matrix& m1, const Matrix& m2);
const Vector3 operator* (const Vector3& v, const Matrix& m);

// Assignment operator overloading
const Matrix& operator*= (const Matrix& m1, const Matrix& m2);

const bool operator== (const Matrix& m1, const Matrix& m2);