#pragma once
#include <DirectXMath.h>

struct Vector2 : public DirectX::XMFLOAT2
{
	Vector2();
	Vector2(const Vector2& vector);
	Vector2(const XMFLOAT2& vector);

	Vector2(const float& x, const float& y);

	Vector2 operator+(Vector2 vector);
	Vector2 operator-(Vector2 vector);
	Vector2 operator*(Vector2 vector);
	Vector2 operator/(Vector2 vector);

	void operator+= (Vector2 vector);
	void operator-= (Vector2 vector);
	void operator*= (Vector2 vector);
	void operator/= (Vector2 vector);

	Vector2 operator+(float f);
	Vector2 operator-(float f);
	Vector2 operator*(float f);
	Vector2 operator/(float f);

	void operator=(float number);

	// Length
	static float Length(Vector2 vector);
	// Normalization
	static Vector2 Normalize(Vector2 vector);
	// Cross Product
	static float Cross(Vector2 v1, Vector2 v2);
	// Dot Product
	static float Dot(Vector2 v1, Vector2 v2);
	// Convert to screen point
	Vector2 Convert(DirectX::XMFLOAT3 pos, DirectX::XMMATRIX viewMat, DirectX::XMMATRIX projMat, int screenW, int screenH, float offsetX, float offsetY);
};

struct Vector3 : public DirectX::XMFLOAT3 
{
	Vector3();
	Vector3(const Vector3& vector);
	Vector3(const XMFLOAT3& vector);
	Vector3(const DirectX::XMVECTOR& vector);
	Vector3(const float num[3]);
	Vector3(const float& num);
	Vector3(const float& x, const float& y, const float& z);

	//Vector3& operator* (Vector3 vector);
	Vector3& operator/ (Vector3 vector);

	Vector3& operator+= (Vector3 vector);
	Vector3& operator-= (Vector3 vector);
	Vector3& operator*= (Vector3 vector);
	Vector3& operator/= (Vector3 vector);

	bool operator!= (Vector3 vector);
	bool operator!= (float num);

	bool operator== (Vector3 vector);
	bool operator== (float num);

	Vector3& operator+= (float f);
	Vector3& operator-= (float f);
	Vector3& operator*= (float f);
	Vector3& operator/= (float f);

	Vector3& operator= (float num);

	// Length
	float Length();

	// Normalization
	Vector3& Normalize();

	// Cross product
	Vector3 Cross(Vector3 vector);

	// Transform
	Vector3 Transform(DirectX::XMMATRIX m, DirectX::XMMATRIX m2);

	// Dot product
	float Dot(Vector3 vector);

	DirectX::XMFLOAT3 ConvertFLOAT3();

	DirectX::XMVECTOR ConvertXMVECTOR();

	static Vector3 Lerp(const Vector3& start, const Vector3& end, float lerpAmount);

	// Length
	static float Length(Vector3 vector);

	// Normalize
	static Vector3 Normalize(Vector3 vector);

	// Cross product
	static Vector3 Cross(Vector3 v1, Vector3 v2);

	// Dot product
	static float Dot(Vector3 v1, Vector3 v2);

	// Convert to XMFLOAT3
	static DirectX::XMFLOAT3 ConvertFLOAT3(const Vector3& vector);
};

const Vector3 operator+ (const Vector3& v1, const Vector3& v2);
const Vector3 operator- (const Vector3& v1, const Vector3& v2);
const Vector3 operator* (const Vector3& v, float s);
const Vector3 operator* (float s, const Vector3& v);
const Vector3 operator* (const Vector3& v1, const Vector3& v2);
const Vector3 operator/ (const Vector3& v, float s);

struct Vector4 : public DirectX::XMFLOAT4 
{
	Vector4();
	Vector4(const Vector4& vector);
	Vector4(const float& x, const float& y, const float& z, const float& w);

	Vector4& operator* (Vector4 vector);
	Vector4& operator/ (Vector4 vector);

	Vector4& operator+= (Vector4 vector);
	Vector4& operator-= (Vector4 vector);
	Vector4& operator*= (Vector4 vector);
	Vector4& operator/= (Vector4 vector);

	Vector4& operator+= (float f);
	Vector4& operator-= (float f);
	Vector4& operator*= (float f);
	Vector4& operator/= (float f);

	Vector4& operator= (float num);

	// Length
	float Length();

	// Normalize
	Vector4& Normalize();

	// Cross Product
	Vector4 Cross(Vector4 vector);

	// Dot product
	float Dot(Vector4 vector);

	DirectX::XMFLOAT3 ConvertFLOAT3();

	// Straight
	static Vector4 Straight(Vector4 pos1, Vector4 pos2);

	// Length
	static float Length(Vector4 vector);

	// Normalize
	static Vector4 Normalize(Vector4 vector);

	// Cross product
	static Vector4 Cross(Vector4 v1, Vector4 v2);

	// Dot product
	static float Dot(Vector4 v1, Vector4 v2);

	// Convert to XMFLOAT3
	static DirectX::XMFLOAT3 ConvertFLOAT3(const Vector4& vector);
};

const Vector4 operator+(const Vector4& v1, const Vector4& v2);
const Vector4 operator-(const Vector4& v1, const Vector4& v2);
const Vector4 operator*(const Vector4& v, float s);
const Vector4 operator*(float s, const Vector4& v);
const Vector4 operator/(const Vector4& v, float s);