#include "Vector.h"
#include "Easing.h"

#pragma region Vector2
Vector2::Vector2()
{
	x = 0;
	y = 0;
}

Vector2::Vector2(const Vector2& vector)
{
	x = vector.x;
	y = vector.y;
}

Vector2::Vector2(const XMFLOAT2& vector)
{
	x = vector.x;
	y = vector.y;
}

Vector2::Vector2(const float& x, const float& y)
{
	this->x = x;
	this->y = y;
}

Vector2 Vector2::operator+(Vector2 vector)
{
	Vector2 ans;
	ans.x = x + vector.x;
	ans.y = y + vector.y;
	return ans;
}

Vector2 Vector2::operator-(Vector2 vector)
{
	Vector2 ans;
	ans.x = x - vector.x;
	ans.y = y - vector.y;
	return ans;
}

Vector2 Vector2::operator*(Vector2 vector)
{
	Vector2 ans;
	ans.x = x * vector.x;
	ans.y = y * vector.y;
	return ans;
}

Vector2 Vector2::operator/(Vector2 vector)
{
	Vector2 ans;
	ans.x = x / vector.x;
	ans.y = y / vector.y;
	return ans;
}

void Vector2::operator+=(Vector2 vector)
{
	x = x + vector.x;
	y = y + vector.y;
}

void Vector2::operator-=(Vector2 vector)
{
	x = x - vector.x;
	y = y - vector.y;
}

void Vector2::operator*=(Vector2 vector)
{
	x = x * vector.x;
	y = y * vector.y;
}

void Vector2::operator/=(Vector2 vector)
{
	x = x / vector.x;
	y = y / vector.y;
}

Vector2 Vector2::operator+(float f)
{
	Vector2 ans;
	ans.x = x + f;
	ans.y = y + f;
	return ans;
}

Vector2 Vector2::operator-(float f)
{
	Vector2 ans;
	ans.x = x - f;
	ans.y = y - f;
	return ans;
}

Vector2 Vector2::operator*(float f)
{
	Vector2 ans;
	ans.x = x * f;
	ans.y = y * f;
	return ans;
}

Vector2 Vector2::operator/(float f)
{
	Vector2 ans;
	ans.x = x / f;
	ans.y = y / f;
	return ans;
}

void Vector2::operator=(float num)
{
	x = num;
	y = num;
}

float Vector2::Length(Vector2 vector)
{
	return sqrt(vector.x * vector.x + vector.y * vector.y);
}

Vector2 Vector2::Normalize(Vector2 vector)
{
	float length = Length(vector);
	if (length != 0)
	{
		Vector2 ans;
		ans.x = vector.x / length;
		ans.y = vector.y / length;
		return ans;
	}
	return vector;
}

float Vector2::Cross(Vector2 v1, Vector2 v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

float Vector2::Dot(Vector2 v1, Vector2 v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}
#pragma endregion

#pragma region Vector3
Vector3::Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}

Vector3::Vector3(const Vector3& vector)
{
	x = vector.x;
	y = vector.y;
	z = vector.z;
}

Vector3::Vector3(const XMFLOAT3& vector)
{
	x = vector.x;
	y = vector.y;
	z = vector.z;
}

Vector3::Vector3(const DirectX::XMVECTOR& vector)
{
	x = vector.m128_f32[0];
	y = vector.m128_f32[1];
	z = vector.m128_f32[2];
}

Vector3::Vector3(const float num[3])
{
	this->x = num[0];
	this->y = num[1];
	this->z = num[2];
}

Vector3::Vector3(const float& num)
{
	x = y = z = num;
}

Vector3::Vector3(const float& x, const float& y, const float& z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3& Vector3::operator*(Vector3 vector)
{
	x = x * vector.x;
	y = y * vector.y;
	z = z * vector.z;
	return *this;
}

Vector3& Vector3::operator/(Vector3 vector)
{
	x = x / vector.x;
	y = y / vector.y;
	z = z / vector.z;
	return *this;
}

Vector3& Vector3::operator+=(Vector3 vector)
{
	x = x + vector.x;
	y = y + vector.y;
	z = z + vector.z;
	return *this;
}

Vector3& Vector3::operator-=(Vector3 vector)
{
	x = x - vector.x;
	y = y - vector.y;
	z = z - vector.z;
	return *this;
}

Vector3& Vector3::operator*=(Vector3 vector)
{
	x = x * vector.x;
	y = y * vector.y;
	z = z * vector.z;
	return *this;
}

Vector3& Vector3::operator/=(Vector3 vector)
{
	x = x / vector.x;
	y = y / vector.y;
	z = z / vector.z;
	return *this;
}

bool Vector3::operator!=(Vector3 vector)
{
	if (x != vector.x) return true;
	if (y != vector.y) return true;
	if (z != vector.z) return true;
	return false;
}

bool Vector3::operator!=(float num)
{
	if (x != num) return true;
	if (y != num) return true;
	if (z != num) return true;
	return false;
}

bool Vector3::operator==(Vector3 vector)
{
	if (x == vector.x && y == vector.y && z == vector.z) return true;
	return false;
}

bool Vector3::operator==(float num)
{
	if (x == num && y == num && z == num) return true;
	return false;
}

Vector3& Vector3::operator+=(float f)
{
	x = x + f;
	y = y + f;
	z = z + f;
	return *this;
}

Vector3& Vector3::operator-=(float f)
{
	x = x - f;
	y = y - f;
	z = z - f;
	return *this;
}

Vector3& Vector3::operator*= (float f)
{
	x = x * f;
	y = y * f;
	z = z * f;
	return *this;
}

Vector3& Vector3::operator/= (float f)
{
	x = x / f;
	y = y / f;
	z = z / f;
	return *this;
}

Vector3& Vector3::operator= (float num)
{
	x = num;
	y = num;
	z = num;
	return *this;
}

float Vector3::Length()
{
	return sqrt(x * x + y * y + z * z);
}

Vector3& Vector3::Normalize()
{
	float length = Length();
	if (length != 0)
	{
		return *this /= length;
	}
	return *this;
}

Vector3 Vector3::Cross(Vector3 vector)
{
	Vector3 ans;
	ans.x = y * vector.z - z * vector.y;
	ans.y = z * vector.x - x * vector.z;
	ans.z = x * vector.y - y * vector.x;
	return ans;
}

float Vector3::Dot(Vector3 vector)
{
	return x * vector.x + y * vector.y + z * vector.z;
}

DirectX::XMFLOAT3 Vector3::ConvertFLOAT3()
{
	return DirectX::XMFLOAT3(x, y, z);
}

DirectX::XMVECTOR Vector3::ConvertXMVECTOR()
{
	DirectX::XMVECTOR vec = { x, y, z, 0 };
	return vec;
}

Vector3 Vector3::Lerp(const Vector3& start, const Vector3& end, float lerpAmount)
{
	Vector3 result;
	result.x = Easing::Lerp(start.x, end.x, lerpAmount);
	result.y = Easing::Lerp(start.y, end.y, lerpAmount);
	result.z = Easing::Lerp(start.z, end.z, lerpAmount);
	return result;
}

float Vector3::Length(Vector3 vector)
{
	return sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

Vector3 Vector3::Normalize(Vector3 vector)
{
	float length = Length(vector);
	if (length != 0)
	{
		return vector /= length;
	}
	return vector;
}

Vector3 Vector3::Cross(Vector3 v1, Vector3 v2)
{
	Vector3 ans;
	ans.x = v1.y * v2.z - v1.z * v2.y;
	ans.y = v1.z * v2.x - v1.x * v2.z;
	ans.z = v1.x * v2.y - v1.y * v2.x;
	return ans;
}

float Vector3::Dot(Vector3 v1, Vector3 v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

DirectX::XMFLOAT3 Vector3::ConvertFLOAT3(const Vector3& vector)
{
	DirectX::XMFLOAT3 convert = { vector.x, vector.y, vector.z };
	return convert;
}

const Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp += v2;
}

const Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp -= v2;
}

const Vector3 operator*(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator*(float s, const Vector3& v)
{
	return v * s;
}

const Vector3 operator*(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp *= v2;
}

const Vector3 operator/(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp /= s;
}
#pragma endregion

#pragma region Vector4
const Vector4 operator*(const Vector4& v, float s)
{
	Vector4 temp(v);
	return temp *= s;
}

Vector4& Vector4::operator*=(float f)
{
	x = x * f;
	y = y * f;
	z = z * f;
	w = w * f;
	return *this;
}

Vector4::Vector4()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

Vector4::Vector4(const Vector4& vector)
{
	x = vector.x;
	y = vector.y;
	z = vector.z;
	w = vector.w;
}

Vector4::Vector4(const float& x, const float& y, const float& z, const float& w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}
#pragma endregion