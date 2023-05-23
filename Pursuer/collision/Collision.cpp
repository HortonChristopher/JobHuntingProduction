#include "Collision.h"

GameWindow* Collision::window = nullptr;

bool Collision::CheckSphere2Sphere(const Sphere& sphereA, const Sphere& sphereB, DirectX::XMVECTOR* inter, DirectX::XMVECTOR* reject)
{
	// If the square of the distance of the center point <= the square of the sum of the radii, then they intersect
	float dist = XMVector3LengthSq(sphereA.center - sphereB.center).m128_f32[0];

	float radius2 = sphereA.radius + sphereB.radius;

	radius2 *= radius2;

	if (dist <= radius2) {
		if (inter) {
			// When the radius of A is 0, the coordinates are the center of B. When the radius of B is 0, the coordinates are complemented to be the center of A.
			float t = sphereB.radius / (sphereA.radius + sphereB.radius);
			*inter = XMVectorLerp(sphereA.center, sphereB.center, t);
		}
		// Calculate the vector to be pushed out
		if (reject) {
			float rejectLen = sphereA.radius + sphereB.radius - sqrtf(dist);
			*reject = XMVector3Normalize(sphereA.center - sphereB.center);
			*reject *= rejectLen;
		}
		return true;
	}

	return false;
}

bool Collision::CheckSphere2Plane(const Sphere& sphere, const Plane& plane, DirectX::XMVECTOR* inter)
{
	// Distance from the origin of the coordinate system to the center coordinates of the sphere
	XMVECTOR distV = XMVector3Dot(sphere.center, plane.normal);
	// By subtracting the origin distance of the plane, the distance between the plane and the center of the sphere is obtained
	float dist = distV.m128_f32[0] - plane.distance;
	// If the absolute distance is larger than the radius, it is not registered as a hit
	if (fabsf(dist) > sphere.radius)
	{
		return false;
	}

	// Calculate the pseudo intersection
	if (inter) 
	{
		// Let the most recent contact on the plane be the pseudo intersection
		*inter = -dist * plane.normal + sphere.center;
	}

	return true;
}

void Collision::ClosestPtPoint2Triangle(const DirectX::XMVECTOR& point, const Triangle& triangle, DirectX::XMVECTOR* closest)
{
	// Check if point is in the outer vertex area of p0
	XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	XMVECTOR p0_p2 = triangle.p2 - triangle.p0;
	XMVECTOR p0_pt = point - triangle.p0;

	XMVECTOR d1 = XMVector3Dot(p0_p1, p0_pt);
	XMVECTOR d2 = XMVector3Dot(p0_p2, p0_pt);

	if (d1.m128_f32[0] <= 0.0f && d2.m128_f32[0] <= 0.0f)
	{
		// p0 is the nearest neighbor
		*closest = triangle.p0;
		return;
	}

	// Check if point is in the outer vertex region of p1
	XMVECTOR p1_pt = point - triangle.p1;

	XMVECTOR d3 = XMVector3Dot(p0_p1, p1_pt);
	XMVECTOR d4 = XMVector3Dot(p0_p2, p1_pt);

	if (d3.m128_f32[0] >= 0.0f && d4.m128_f32[0] <= d3.m128_f32[0])
	{
		// p1 is the nearest neighbor
		*closest = triangle.p1;
		return;
	}

	// Checks if point is in the edge area of p0_p1, and if so, returns the projection of point on p0_p1
	float vc = d1.m128_f32[0] * d4.m128_f32[0] - d3.m128_f32[0] * d2.m128_f32[0];

	if (vc <= 0.0f && d1.m128_f32[0] >= 0.0f && d3.m128_f32[0] <= 0.0f)
	{
		float v = d1.m128_f32[0] / (d1.m128_f32[0] - d3.m128_f32[0]);
		*closest = triangle.p0 + v * p0_p1;
		return;
	}

	// Check if point is in the outer vertex region of p2
	XMVECTOR p2_pt = point - triangle.p2;

	XMVECTOR d5 = XMVector3Dot(p0_p1, p2_pt);
	XMVECTOR d6 = XMVector3Dot(p0_p2, p2_pt);

	if (d6.m128_f32[0] >= 0.0f && d5.m128_f32[0] <= d6.m128_f32[0])
	{
		*closest = triangle.p2;
		return;
	}

	// Checks if point is in the edge area of p0_p2 and returns the projection of point on p0_p2
	float vb = d5.m128_f32[0] * d2.m128_f32[0] - d1.m128_f32[0] * d6.m128_f32[0];

	if (vb <= 0.0f && d2.m128_f32[0] >= 0.0f && d6.m128_f32[0] <= 0.0f)
	{
		float w = d2.m128_f32[0] / (d2.m128_f32[0] - d6.m128_f32[0]);
		*closest = triangle.p0 + w * p0_p2;
		return;
	}

	// Checks if point is in the edge area of p1_p2, and if so, returns the projection of point on p1_p2
	float va = d3.m128_f32[0] * d6.m128_f32[0] - d5.m128_f32[0] * d4.m128_f32[0];

	if (va <= 0.0f && (d4.m128_f32[0] - d3.m128_f32[0]) >= 0.0f && (d5.m128_f32[0] - d6.m128_f32[0]) >= 0.0f)
	{
		float w = (d4.m128_f32[0] - d3.m128_f32[0]) / ((d4.m128_f32[0] - d3.m128_f32[0]) + (d5.m128_f32[0] - d6.m128_f32[0]));
		*closest = triangle.p1 + w * (triangle.p2 - triangle.p1);
		return;
	}

	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;
}

bool Collision::CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, DirectX::XMVECTOR* inter, DirectX::XMVECTOR* reject)
{
	XMVECTOR p;

	// Find the point p on the triangle that is the most recent point of contact with the center of the sphere
	ClosestPtPoint2Triangle(sphere.center, triangle, &p);

	// Difference vector between point p and the center of the sphere
	XMVECTOR v = p - sphere.center;

	// Find the square of the distance
	// The inner product of the same vectors matches the equation inside the root of the three-square theorem
	float distanceSquare = XMVector3Dot(v, v).m128_f32[0];
	// ‹…‚ÆŽOŠpŒ`‚Ì‹——£”¼ŒaˆÈ‰º‚È‚ç“–‚½‚Á‚Ä‚¢‚È‚¢ If it is less than the radius of the distance between the sphere and the triangle, it is not registered as a hit.
	if (distanceSquare > sphere.radius * sphere.radius) return false;

	// Calculate pseudo intersection
	if (inter) {
		// Let the most recent contact p on the triangle be a pseudo intersection
		*inter = p;
	}
	// Calculate the vector to be pushed out
	if (reject) {
		float ds = XMVector3Dot(sphere.center, triangle.normal).m128_f32[0];
		float dt = XMVector3Dot(triangle.p0, triangle.normal).m128_f32[0];
		float rejectLen = dt - ds + sphere.radius;

		*reject = triangle.normal * rejectLen;
	}

	return true;
}

bool Collision::CheckRay2Plane(const Ray& ray, const Plane& plane, float* distance, DirectX::XMVECTOR* inter)
{
	const float epsilon = 1.0e-5f; // Small value for error absorption

	// Dot product of plane line and ray direction vector
	float d1 = XMVector3Dot(plane.normal, ray.dir).m128_f32[0];

	// Does not hit the back
	if (d1 > -epsilon) { return false; }

	// Distance between start point and origin (normal direction of plane)
	// Inner product of surface normal and ray start point coordinates (position vector)
	float d2 = XMVector3Dot(plane.normal, ray.start).m128_f32[0];

	// Distance between the start point and the plane (normal direction of the plane)
	float dist = d2 - plane.distance;

	// Distance between start point and plane (ray direction)
	float t = dist / -d1;

	// The intersection is behind the starting point, so it won't hit
	if (t < 0)
	{
		return false;
	}

	// Write the distance
	if (distance) { *distance = t; }

	// Calculate intersection
	if (inter) { *inter = ray.start + t * ray.dir; }

	return true;
}

bool Collision::CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance, DirectX::XMVECTOR* inter)
{
	// Calculate the plane on which the triangle rests
	Plane plane;
	XMVECTOR interPlane;
	plane.normal = triangle.normal;
	plane.distance = XMVector3Dot(triangle.normal, triangle.p0).m128_f32[0];

	// If the plane does not hit the ray, it does not register a hit
	if (!CheckRay2Plane(ray, plane, distance, &interPlane)) { return false; }

	// Since the ray and the plane were in contact, the distance and the intersection was written.
	// Determine if the intersection of the ray and the plane is inside the triangle
	const float epsilon = 1.0e-5f;	// Small value for error absorption
	XMVECTOR m;

	// About edge p0_p1
	XMVECTOR pt_p0 = triangle.p0 - interPlane;
	XMVECTOR p0_p1 = triangle.p1 - triangle.p0;
	m = XMVector3Cross(pt_p0, p0_p1);

	// If it is outside the side, it does not hit, so the judgment is terminated
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) 
	{
		return false;
	}

	// About edge p1_p2
	XMVECTOR pt_p1 = triangle.p1 - interPlane;
	XMVECTOR p1_p2 = triangle.p2 - triangle.p1;

	m = XMVector3Cross(pt_p1, p1_p2);
	// If it is outside the side, it does not hit, so the judgment is terminated
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) 
	{
		return false;
	}

	// About edge p2_p0
	XMVECTOR pt_p2 = triangle.p2 - interPlane;
	XMVECTOR p2_p0 = triangle.p0 - triangle.p2;

	m = XMVector3Cross(pt_p2, p2_p0);
	// If it is outside the side, it does not hit, so the judgment is terminated
	if (XMVector3Dot(m, triangle.normal).m128_f32[0] < -epsilon) 
	{
		return false;
	}

	// Because it's inside, it's hit
	if (inter) 
	{
		*inter = interPlane;
	}

	return true;
}

bool Collision::CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float* distance, DirectX::XMVECTOR* inter)
{
	XMVECTOR m = ray.start - sphere.center;
	float b = XMVector3Dot(m, ray.dir).m128_f32[0];
	float c = XMVector3Dot(m, m).m128_f32[0] - sphere.radius * sphere.radius;

	// If the starting point of the lay is outside the sphere (c> 0) and the lay points away from the sphere (b> 0), it will not hit.
	if (c > 0.0f && b > 0.0f) return false;

	float discr = b * b - c;

	// Negative discriminant agrees that Ray is off the ball
	if (discr < 0.0f) { return false; }

	// Ray intersects the sphere
	// Calculate the minimum value t to intersect
	float t = -b - sqrtf(discr);

	// If t is negative, the ray starts from the inside of the sphere, so clamp t to zero.
	if (t < 0) t = 0.0f;
	if (distance) { *distance = t; }

	if (inter) { *inter = ray.start + t * ray.dir; }

	return true;
}

bool Collision::CheckRay2OBB(const Ray& ray, const Box& obb, float* distance, DirectX::XMVECTOR* inter)
{
	Plane plane;
	float scale[3] = { obb.scale.x ,obb.scale.y ,obb.scale.z };
	bool result = false;
	float resultDistance = D3D12_FLOAT32_MAX;

	XMVECTOR resultInter = {};
	int resultInt = 0;

	// OBB 6 faces, face and ray determination.
	for (int i = 0; i < 6; i++)
	{
		XMVECTOR otherDir[2];
		// X direction 1 when i is 0,3, Y direction 2 when i is 4, Z direction when i is 5
		int val2 = i % 3;

		if (val2 == 0)
		{
			otherDir[0] = obb.normaDirect[1] * obb.scale.y;
			otherDir[1] = obb.normaDirect[2] * obb.scale.z;
		}
		else if (val2 == 1)
		{
			otherDir[0] = obb.normaDirect[0] * obb.scale.x;
			otherDir[1] = obb.normaDirect[2] * obb.scale.z;
		}
		else
		{
			otherDir[0] = obb.normaDirect[0] * obb.scale.x;
			otherDir[1] = obb.normaDirect[1] * obb.scale.y;
		}

		// 0-2 is positive direction to the axis, 3-5 is negative direction
		int val = i / 3;
		plane.normal = obb.normaDirect[i - 3 * val] * (1 - 2 * val);

		XMVECTOR p0 = otherDir[0] + otherDir[1] + obb.normaDirect[i - 3 * val] * (1 - 2 * val) * scale[i - 3 * val] + obb.center;

		plane.distance = XMVector3Dot(plane.normal, p0).m128_f32[0];

		float dis;
		XMVECTOR interPlane;

		// If the ray and plane are not hit, they are not hit
		if (!CheckRay2Plane(ray, plane, &dis, &interPlane))
		{
			continue;
		}

		// Write information if the collision distance is smaller than the existing one
		if (resultDistance > dis)
		{
			resultDistance = dis;
			resultInter = interPlane;
			resultInt = i;
		}
		result = true;
	}

	if (!result)
	{
		return false;
	}

	// 0-2 is positive direction to the axis, 3-5 is negative direction
	int val = resultInt / 3;

	// X direction 1 when i is 0,3, Y direction 2 when i is 4, Z direction when i is 5
	int val2 = resultInt % 3;
	auto normal = obb.normaDirect[resultInt - 3 * val] * (1 - 2 * val);
	const float epsilon = 1.0e-5f; // Minute value for error absorption 

	float normalScale = {};

	XMVECTOR otherDir[2];

	if (val2 == 0)
	{
		normalScale = obb.scale.x;
		otherDir[0] = obb.normaDirect[1] * obb.scale.y;
		otherDir[1] = obb.normaDirect[2] * obb.scale.z;
	}
	else if (val2 == 1)
	{
		normalScale = obb.scale.y;
		otherDir[0] = obb.normaDirect[0] * obb.scale.x;
		otherDir[1] = obb.normaDirect[2] * obb.scale.z;
	}
	else
	{
		normalScale = obb.scale.z;
		otherDir[0] = obb.normaDirect[0] * obb.scale.x;
		otherDir[1] = obb.normaDirect[1] * obb.scale.y;
	}

	XMVECTOR p[4];
	p[0] = otherDir[0] + otherDir[1] + normal * normalScale + obb.center;
	p[1] = otherDir[0] - otherDir[1] + normal * normalScale + obb.center;
	p[2] = -otherDir[0] - otherDir[1] + normal * normalScale + obb.center;
	p[3] = -otherDir[0] + otherDir[1] + normal * normalScale + obb.center;

	// OK if the ray is through the inside in either right or left turn.
	bool check1 = true;
	bool check2 = true;

	XMVECTOR m;
	// For the edge p0_p1
	XMVECTOR pt_p0 = p[0] - resultInter;
	XMVECTOR p0_p1 = p[1] - p[0];

	m = XMVector3Cross(pt_p0, p0_p1);
	// Outer side of the edge
	if (XMVector3Dot(m, normal).m128_f32[0] < -epsilon)
	{
		check1 = false;
	}

	if (check1)
	{
		// For edge p1_p2
		XMVECTOR pt_p1 = p[1] - resultInter;
		XMVECTOR p1_p2 = p[2] - p[1];
		m = XMVector3Cross(pt_p1, p1_p2);
		// Outer side of the edge
		if (XMVector3Dot(m, normal).m128_f32[0] < -epsilon)
		{
			check1 = false;
		}
	}
	if (check1)
	{
		// For side p2_p3
		XMVECTOR pt_p2 = p[2] - resultInter;
		XMVECTOR p2_p3 = p[3] - p[2];
		m = XMVector3Cross(pt_p2, p2_p3);
		// Outer side of the edge
		if (XMVector3Dot(m, normal).m128_f32[0] < -epsilon)
		{
			check1 = false;
		}
	}
	if (check1)
	{
		// For edge p3_p0
		XMVECTOR pt_p3 = p[3] - resultInter;
		XMVECTOR p3_p0 = p[0] - p[3];
		m = XMVector3Cross(pt_p3, p3_p0);
		// Outer side of the edge
		if (XMVector3Dot(m, normal).m128_f32[0] < -epsilon)
		{
			check1 = false;
		}
	}

	if (!check1)
	{
		if (check2)
		{
			// For the edge p0_p1
			XMVECTOR pt_p0 = p[0] - resultInter;
			XMVECTOR p0_p1 = p[0] - p[1];
			m = XMVector3Cross(pt_p0, p0_p1);
			// Outer side of the edge
			if (XMVector3Dot(m, normal).m128_f32[0] < -epsilon) 
			{
				check2 = false;
			}
		}
		if (check2)
		{
			// For edge p1_p2
			XMVECTOR pt_p1 = p[1] - resultInter;
			XMVECTOR p1_p2 = p[1] - p[2];
			m = XMVector3Cross(pt_p1, p1_p2);
			// For side p2_p3
			if (XMVector3Dot(m, normal).m128_f32[0] < -epsilon) 
			{
				check2 = false;
			}
		}
		if (check2)
		{
			// For side p2_p3
			XMVECTOR pt_p2 = p[2] - resultInter;
			XMVECTOR p2_p3 = p[2] - p[3];
			m = XMVector3Cross(pt_p2, p2_p3);
			// Outer side of the edge
			if (XMVector3Dot(m, normal).m128_f32[0] < -epsilon) 
			{
				check2 = false;
			}
		}
		if (check2)
		{
			// For edge p3_p0
			XMVECTOR pt_p3 = p[3] - resultInter;
			XMVECTOR p3_p0 = p[3] - p[0];
			m = XMVector3Cross(pt_p3, p3_p0);
			// Outer side of the edge
			if (XMVector3Dot(m, normal).m128_f32[0] < -epsilon) 
			{
				check2 = false;
			}
		}
	}

	// If you go right or left, if you go both ways on the outside, you haven't hit anything.
	if (!check1 && !check2)
	{
		return false;
	}

	if (inter)
	{
		*inter = resultInter;
	}

	if (distance)
	{
		*distance = resultDistance;
	}

	// Collision detected
	return true;
}

// OBB-OBB
bool Collision::CheckOBBOBB(const Box& box1, const Box& box2, DirectX::XMVECTOR* inter, DirectX::XMVECTOR* reject)
{

	auto scale1 = box1.scale;
	auto scale2 = box2.scale;

	// Secure each direction vector
	// (N***: Standardized direction vector)
	Vector3 NAe1 = box1.normaDirect[0];
	Vector3 NAe2 = box1.normaDirect[1];
	Vector3 NAe3 = box1.normaDirect[2];
	Vector3 NBe1 = box2.normaDirect[0];
	Vector3 NBe2 = box2.normaDirect[1];
	Vector3 NBe3 = box2.normaDirect[2];

	Vector3 Ae1 = NAe1 * scale1.x;
	Vector3 Ae2 = NAe2 * scale1.y;
	Vector3 Ae3 = NAe3 * scale1.z;
	Vector3 Be1 = NBe1 * scale2.x;
	Vector3 Be2 = NBe2 * scale2.y;
	Vector3 Be3 = NBe3 * scale2.z;

	Vector3 Interval = box1.center - box2.center;

	float rejectLength = 0;
	Vector3 rejectVec = {};

	// Axis of separation : Ae1
	float rA = Ae1.Length();
	float rB = LengthSegmentOnSeparateAxis(NAe1, Be1, Be2, Be3);
	float L = fabs(Vector3::Dot(Interval, NAe1));
	if (L > rA + rB)
		return false; // Õ“Ë‚µ‚Ä‚¢‚È‚¢

	// Axis of separation : Ae2
	rA = Ae2.Length();
	rB = LengthSegmentOnSeparateAxis(NAe2, Be1, Be2, Be3);
	L = fabs(Vector3::Dot(Interval, NAe2));
	if (L > rA + rB)
		return false;

	// Axis of separation : Ae3
	rA = Ae3.Length();
	rB = LengthSegmentOnSeparateAxis(NAe3, Be1, Be2, Be3);
	L = fabs(Vector3::Dot(Interval, NAe3));
	if (L > rA + rB)
		return false;

	// Axis of separation : Be1
	rA = LengthSegmentOnSeparateAxis(NBe1, Ae1, Ae2, Ae3);
	rB = Be1.Length();
	L = fabs(Vector3::Dot(Interval, NBe1));
	if (L > rA + rB)
		return false;
	rejectLength = rB + rA - L;
	rejectVec = NBe1;

	// Axis of separation : Be2
	rA = LengthSegmentOnSeparateAxis(NBe2, Ae1, Ae2, Ae3);
	rB = Be2.Length();
	L = fabs(Vector3::Dot(Interval, NBe2));
	if (L > rA + rB)
		return false;
	if (rejectLength > rB + rA - L)
	{
		rejectLength = rB + rA - L;
		rejectVec = NBe2;
	}

	// Axis of separation : Be3
	rA = LengthSegmentOnSeparateAxis(NBe3, Ae1, Ae2, Ae3);
	rB = Be3.Length();
	L = fabs(Vector3::Dot(Interval, NBe3));
	if (L > rA + rB)
		return false;
	if (rejectLength > rB + rA - L)
	{
		rejectLength = rB + rA - L;
		rejectVec = NBe3;
	}

	// Axis of separation : C11
	Vector3 Cross;
	Cross = Vector3::Cross(NAe1, NBe1);
	rA = LengthSegmentOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LengthSegmentOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Vector3::Dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// Axis of separation : C12
	Cross = Vector3::Cross(NAe1, NBe2);
	rA = LengthSegmentOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LengthSegmentOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Vector3::Dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// Axis of separation : C13
	Cross = Vector3::Cross(NAe1, NBe3);
	rA = LengthSegmentOnSeparateAxis(Cross, Ae2, Ae3);
	rB = LengthSegmentOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Vector3::Dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// Axis of separation : C21
	Cross = Vector3::Cross(NAe2, NBe1);
	rA = LengthSegmentOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LengthSegmentOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Vector3::Dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// Axis of separation : C22
	Cross = Vector3::Cross(NAe2, NBe2);
	rA = LengthSegmentOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LengthSegmentOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Vector3::Dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// Axis of separation : C23
	Cross = Vector3::Cross(NAe2, NBe3);
	rA = LengthSegmentOnSeparateAxis(Cross, Ae1, Ae3);
	rB = LengthSegmentOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Vector3::Dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// Axis of separation : C31
	Cross = Vector3::Cross(NAe3, NBe1);
	rA = LengthSegmentOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LengthSegmentOnSeparateAxis(Cross, Be2, Be3);
	L = fabs(Vector3::Dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// Axis of separation : C32
	Cross = Vector3::Cross(NAe3, NBe2);
	rA = LengthSegmentOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LengthSegmentOnSeparateAxis(Cross, Be1, Be3);
	L = fabs(Vector3::Dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	// Axis of separation : C33
	Cross = Vector3::Cross(NAe3, NBe3);
	rA = LengthSegmentOnSeparateAxis(Cross, Ae1, Ae2);
	rB = LengthSegmentOnSeparateAxis(Cross, Be1, Be2);
	L = fabs(Vector3::Dot(Interval, Cross));
	if (L > rA + rB)
		return false;

	if (reject)
	{
		// If the exclusion vector and the direction vector of Box1-2 are opposite, reverse the exclusion vector.
		float dot = rejectVec.Dot(Vector3::Normalize(Interval));

		if (dot < 0)
		{
			rejectVec *= -1;
		}

		*reject = Vector3(rejectVec * rejectLength).ConvertXMVECTOR();
	}

	// Since there are no separation planes, they collide
	return true;
}

float Collision::LengthSegmentOnSeparateAxis(const Vector3& Sep, const Vector3& e1, const Vector3& e2, const Vector3& e3)
{
	// Calculate the projected line segment length by summing the absolute values of the three inner products
	// Separation axis Sep must be standardized
	float r1 = fabs(Vector3::Dot(Sep, e1));
	float r2 = fabs(Vector3::Dot(Sep, e2));
	float r3;

	if (e3.x != 0 || e3.y != 0 || e3.z != 0)
	{
		r3 = fabs(Vector3::Dot(Sep, e3));
	}
	else
	{
		r3 = 0;
	}

	return r1 + r2 + r3;
}

bool Collision::CheckSphereCapsule(const Sphere& sphere, const Capsule& capsule)
{
	Vector3 vStartToEnd = capsule.endPosition - capsule.startPosition;

	Vector3 n = vStartToEnd;
	n.Normalize();

	const float t = Vector3(sphere.center - capsule.startPosition).Dot(n);

	const Vector3 vPsPn = n * t;
	const Vector3 posPn = vPsPn + capsule.startPosition;

	const float lengthRate = t / vStartToEnd.Length();

	float distance;

	if (lengthRate < 0.0f)
	{
		distance = Vector3(sphere.center - capsule.startPosition).Length() - capsule.radius;
	}
	else if (lengthRate <= 1.0f)
	{
		distance = Vector3(sphere.center - posPn).Length() - capsule.radius;
	}
	else
	{
		distance = Vector3(sphere.center - capsule.endPosition).Length() - capsule.radius;
	}

	if (sphere.radius > distance)
	{
		return true;
	}
	return false;
}

bool Collision::CheckOBBPlane(const Box& obb, const Plane& plane, DirectX::XMVECTOR* inter, DirectX::XMVECTOR* reject)
{
	// Calculate the length of the projection line of the OBB relative to the normal of the plane
	float r = 0.0f; // Radius
	Vector3 PlaneNormal = plane.normal; // Normal vector of a plane
	int i;

	for (i = 0; i < 3; i++) {

		float directLength = 0.0f;

		switch (i)
		{
		case 0:
			directLength = obb.scale.x;
			break;
		case 1:
			directLength = obb.scale.y;
			break;
		case 2:
			directLength = obb.scale.z;
			break;
		default:
			break;
		}

		Vector3 Direct = obb.normaDirect[i]; // One axis vector of OBB
		r += fabs(Vector3::Dot((Direct * directLength), PlaneNormal));
	}

	// Calculate distance between plane and OBB
	Vector3 ObbPos = obb.center;
	Vector3 PlanePos = Vector3(0, 0, plane.distance);
	float s = Vector3::Dot((ObbPos - PlanePos), PlaneNormal);

	// Collision detection
	if (fabs(s) - r >= 0.0f)
	{
		return false; // No collision
	}

	// Calculate return distance
	if (reject) {
		if (s > 0)
		{
			*reject = Vector3(PlaneNormal * (r - fabs(s))).ConvertXMVECTOR();
		}
		else
		{
			*reject = Vector3(PlaneNormal * (r + fabs(s))).ConvertXMVECTOR();
		}
	}

	return true; // Colliding
}