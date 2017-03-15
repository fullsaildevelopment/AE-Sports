#include "CollisionLibrary.h"
#include <math.h>
#include <iostream>

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#define clamp(a) (max(0,min(1,a)))

#ifndef SameSign
#define SameSign(a,b) ( ((*(unsigned int*)&(a)) & 0x80000000) == ((*(unsigned int*)&(b)) & 0x80000000) )
#endif 

bool isZero(float3 v)
{
	if (v.x <= FLT_EPSILON && v.x >= -FLT_EPSILON)
	{
		if (v.y <= FLT_EPSILON && v.y >= -FLT_EPSILON)
		{
			if (v.z <= FLT_EPSILON && v.z >= -FLT_EPSILON)
			{
				return true;
			}
		}
	}
	return false;
}

// ComputePlane
//
// Calculate the plane normal and plane offset from the input points
void ComputePlane(Plane &plane, const vec3f& pointA, const vec3f& pointB, const vec3f &pointC)
{
	cross_product(plane.normal, pointB - pointA, pointC - pointB);
	plane.normal.normalize();
	plane.offset = dot_product(pointA, plane.normal);
}

// ClassifyPointToPlane
//
// Perform a half-space test. Returns 1 if the point is on or in front of the plane.
// Returns 2 if the point is behind the plane.
int ClassifyPointToPlane(const Plane& plane, const vec3f& point)
{
	return (dot_product(point, plane.normal) >= plane.offset) ? 1 : 2;
}
// Returns 0 if on plane 1 if in front of -1 if behind
int ClassifyPointToPlane(const NewPlane& plane, const vec3f& point)
{
	float r = dot_product(plane.p - point, plane.n);
	if (r <= 0.00001f && r >= -0.00001f)
	{
		return 0;
	}
	else if (r < 0) return 1;
	else return -1;
}

//If d == 0  then it gives you the point at which it crosses the plane else it just gives you the point on the line at that percentage
float3 LS2PI(const float3& p0, const float3& p1, const NewPlane& plane, float& d)
{
	float3 L = p1 - p0;
	d = dot_product(plane.p - p0, plane.n) / dot_product(plane.n, L);
	return p0 + L * d;
}

// ClassifySphereToPlane
//
// Perform a sphere-to-plane test. 
// Returns 1 if the sphere is in front of the plane.
// Returns 2 if the sphere is behind the plane.
// Returns 3 if the sphere straddles the plane.
int ClassifySphereToPlane(const Plane& plane, const Sphere& sphere)
{
	float SOffset = dot_product(sphere.m_Center, plane.normal) - plane.offset;
	if (sphere.m_Radius < abs(SOffset))
	{
		if (SOffset > 0)
			return 1;
		else
			return 2;
	}
	return 3;
}

int ClassifySphereToPlane(const NewPlane& p, const Sphere& sphere)
{
	float SOffset = dot_product(sphere.m_Center - p.p, p.n);
	if (sphere.m_Radius < abs(SOffset))
	{
		if (SOffset > 0)
			return 1;
		else
			return 2;
	}
	return 3;
}

// ClassifyAabbToPlane
//
// Performs a AABB-to-plane test.
// Returns 1 if the aabb is in front of the plane.
// Returns 2 if the aabb is behind the plane.
// Returns 3 if the aabb straddles the plane.
int ClassifyAabbToPlane(const Plane& plane, const AABB& aabb)
{
	Sphere sphere;
	sphere.m_Center = (aabb.min + aabb.max) * 0.5f;
	sphere.m_Radius = dot_product(aabb.max - sphere.m_Center, vec3f(abs(plane.normal.x), abs(plane.normal.y), abs(plane.normal.z)));
	return ClassifySphereToPlane(plane, sphere);
}

// ClassifyCapsuleToPlane
//
// Performs a Capsule-to-plane test.
// Returns 1 if the aabb is in front of the plane.
// Returns 2 if the aabb is behind the plane.
// Returns 3 if the aabb straddles the plane.
int ClassifyCapsuleToPlane(const Plane& plane, const Capsule& capsule)
{
	Sphere S1;
	Sphere S2;
	S1.m_Center = capsule.m_Segment.m_Start;
	S2.m_Center = capsule.m_Segment.m_End;
	S1.m_Radius = S2.m_Radius = capsule.m_Radius;
	int s1R = ClassifySphereToPlane(plane, S1);
	int s2R = ClassifySphereToPlane(plane, S2);
	return (s1R == 1 && s2R == 1) ? 1 : (s1R == 2 && s2R == 2) ? 2 : 3;
}

// BuildFrustum
//
// Calculates the corner points and planes of the frustum based upon input values.
void BuildFrustum(Frustum& frustum, float fov, float nearDist, float farDist, float ratio, const matrix4f& camXform)
{
	// TO DO:
	// Calculate the 8 corner points of the frustum and store them in the frustum.corners[] array.
	// Use the FrustumCorners enum in CollisionLibrary.h to index into the corners array.
	vec3f NC = camXform.axis_pos - camXform.axis_z * nearDist;
	vec3f FC = camXform.axis_pos - camXform.axis_z * farDist;

	float Hnear = tan(fov * 0.5f) * nearDist;
	float Hfar = tan(fov * 0.5f) * farDist;
	float Wnear = Hnear * ratio;
	float Wfar = Hfar * ratio;
	vec3f FcamY = camXform.axis_y * Hfar;
	vec3f NcamY = camXform.axis_y * Hnear;
	vec3f FcamX = camXform.axis_x * Wfar;
	vec3f NcamX = camXform.axis_x * Wnear;
	frustum.corners[FTL] = FC + FcamY - FcamX;
	frustum.corners[FTR] = FC + FcamY + FcamX;
	frustum.corners[FBL] = FC - FcamY - FcamX;
	frustum.corners[FBR] = FC - FcamY + FcamX;
	frustum.corners[NTL] = NC + NcamY - NcamX;
	frustum.corners[NTR] = NC + NcamY + NcamX;
	frustum.corners[NBL] = NC - NcamY - NcamX;
	frustum.corners[NBR] = NC - NcamY + NcamX;

	// Use the corner points to calculate the frustum planes.
	// This step is completed for you.
	ComputePlane(frustum.planes[NEAR_PLANE], frustum.corners[NBR], frustum.corners[NBL], frustum.corners[NTL]);
	ComputePlane(frustum.planes[FAR_PLANE], frustum.corners[FBL], frustum.corners[FBR], frustum.corners[FTR]);
	ComputePlane(frustum.planes[LEFT_PLANE], frustum.corners[NBL], frustum.corners[FBL], frustum.corners[FTL]);
	ComputePlane(frustum.planes[RIGHT_PLANE], frustum.corners[FBR], frustum.corners[NBR], frustum.corners[NTR]);
	ComputePlane(frustum.planes[TOP_PLANE], frustum.corners[NTR], frustum.corners[NTL], frustum.corners[FTL]);
	ComputePlane(frustum.planes[BOTTOM_PLANE], frustum.corners[NBL], frustum.corners[NBR], frustum.corners[FBR]);
}

// FrustumToSphere
//
// Perform a Sphere-to-Frustum check. Returns true if the sphere is inside. False if not.
bool FrustumToSphere(const Frustum& frustum, const Sphere& sphere)
{
	if (ClassifySphereToPlane(frustum.planes[NEAR_PLANE], sphere) == 2) return false;
	if (ClassifySphereToPlane(frustum.planes[FAR_PLANE], sphere) == 2) return false;
	if (ClassifySphereToPlane(frustum.planes[LEFT_PLANE], sphere) == 2) return false;
	if (ClassifySphereToPlane(frustum.planes[RIGHT_PLANE], sphere) == 2) return false;
	if (ClassifySphereToPlane(frustum.planes[TOP_PLANE], sphere) == 2) return false;
	if (ClassifySphereToPlane(frustum.planes[BOTTOM_PLANE], sphere) == 2) return false;
	return true;
}

// FrustumToAABB
//
// Perform a Aabb-to-Frustum check. Returns true if the aabb is inside. False if not.
bool FrustumToAABB(const Frustum& frustum, const AABB& aabb)
{
	if (ClassifyAabbToPlane(frustum.planes[NEAR_PLANE], aabb) == 2) return false;
	if (ClassifyAabbToPlane(frustum.planes[FAR_PLANE], aabb) == 2) return false;
	if (ClassifyAabbToPlane(frustum.planes[LEFT_PLANE], aabb) == 2) return false;
	if (ClassifyAabbToPlane(frustum.planes[RIGHT_PLANE], aabb) == 2) return false;
	if (ClassifyAabbToPlane(frustum.planes[TOP_PLANE], aabb) == 2) return false;
	if (ClassifyAabbToPlane(frustum.planes[BOTTOM_PLANE], aabb) == 2) return false;
	return true;
}

// FrustumToCapsule
//
// Perform a Capsule-to-Frustum check. Returns true if the Capsule is inside. False if not.
bool FrustumToCapsule(const Frustum& frustum, const Capsule& capsule)
{
	if (ClassifyCapsuleToPlane(frustum.planes[NEAR_PLANE], capsule) == 2) return false;
	if (ClassifyCapsuleToPlane(frustum.planes[FAR_PLANE], capsule) == 2) return false;
	if (ClassifyCapsuleToPlane(frustum.planes[LEFT_PLANE], capsule) == 2) return false;
	if (ClassifyCapsuleToPlane(frustum.planes[RIGHT_PLANE], capsule) == 2) return false;
	if (ClassifyCapsuleToPlane(frustum.planes[TOP_PLANE], capsule) == 2) return false;
	if (ClassifyCapsuleToPlane(frustum.planes[BOTTOM_PLANE], capsule) == 2) return false;
	return true;
}

// AABBtoAABB
//
// Returns true if the AABBs collide. False if not.
bool AABBtoAABB(const AABB& lhs, const AABB& rhs)
{
	if (lhs.max.x < rhs.min.x || lhs.min.x > rhs.max.x) return false;
	if (lhs.max.y < rhs.min.y || lhs.min.y > rhs.max.y) return false;
	if (lhs.max.z < rhs.min.z || lhs.min.z > rhs.max.z) return false;
	return true;
}

// SphereToSphere
//
// Returns true if the Spheres collide. False if not.
bool SphereToSphere(const Sphere& lhs, const Sphere& rhs)
{
	if (dot_product(lhs.m_Center - rhs.m_Center, lhs.m_Center - rhs.m_Center) > powf(lhs.m_Radius + rhs.m_Radius, 2))
		return false;
	return true;
}

// SphereToAABB
//
// Returns true if the sphere collides with the AABB. False if not.
bool SphereToAABB(const Sphere& lhs, const AABB& rhs)
{
	vec3f cp = lhs.m_Center;
	if (lhs.m_Center.x < rhs.min.x)
		cp.x = rhs.min.x;
	if (lhs.m_Center.y < rhs.min.y)
		cp.y = rhs.min.y;
	if (lhs.m_Center.z < rhs.min.z)
		cp.z = rhs.min.z;
	if (lhs.m_Center.x > rhs.max.x)
		cp.x = rhs.max.x;
	if (lhs.m_Center.y > rhs.max.y)
		cp.y = rhs.max.y;
	if (lhs.m_Center.z > rhs.max.z)
		cp.z = rhs.max.z;

	if (dot_product(lhs.m_Center - cp, lhs.m_Center - cp) > powf(lhs.m_Radius, 2))
		return false;
	return true;
}

// CapsuleToSphere
//
// Returns true if the capsule collides with the sphere. False if not.
bool CapsuleToSphere(const Capsule& capsule, const Sphere& sphere)
{
	vec3f SE = capsule.m_Segment.m_End - capsule.m_Segment.m_Start;
	float ratio = dot_product(SE, sphere.m_Center - capsule.m_Segment.m_Start) / dot_product(SE, SE);
	ratio = max(0, min(ratio, 1));
	SE = capsule.m_Segment.m_Start + SE * ratio;
	if (dot_product(sphere.m_Center - SE, sphere.m_Center - SE) > powf(sphere.m_Radius + capsule.m_Radius, 2))
		return false;
	return true;
}

bool AABBToCapsule(const AABB& box, const Capsule& cap)
{
	Sphere sphere;
	sphere.m_Center = (box.min + box.max) * 0.5f;
	vec3f cp = cap.m_Segment.m_End - cap.m_Segment.m_Start;
	float ratio = dot_product(cp, sphere.m_Center - cap.m_Segment.m_Start) / dot_product(cp, cp);
	ratio = min(max(ratio, 1), 0);
	cp = cap.m_Segment.m_Start + cp * ratio;
	vec3f cpb = cp;
	if (cp.x < box.min.x)
		cpb.x = box.min.x;
	if (cp.y < box.min.y)
		cpb.y = box.min.y;
	if (cp.z < box.min.z)
		cpb.z = box.min.z;
	if (cp.x > box.max.x)
		cpb.x = box.max.x;
	if (cp.y > box.max.y)
		cpb.y = box.max.y;
	if (cp.z > box.max.z)
		cpb.z = box.max.z;

	if (dot_product(cp - cpb, cp - cpb) > powf(cap.m_Radius, 2))
		return false;
	return true;

}

bool CapsuleToCapsule(const Capsule& capl, const Capsule& capr)
{
	AABB bl;
	AABB br;
	bl.min = capl.m_Segment.m_Start - float3(capl.m_Radius, capl.m_Radius, capl.m_Radius);
	bl.max = capl.m_Segment.m_End + float3(capl.m_Radius, capl.m_Radius, capl.m_Radius);

	br.min = capr.m_Segment.m_Start - float3(capr.m_Radius, capr.m_Radius, capr.m_Radius);
	br.max = capr.m_Segment.m_End + float3(capr.m_Radius, capr.m_Radius, capr.m_Radius);

	if (!AABBtoAABB(bl, br)) return false;

	float3 cpl;
	float3 cpr;

	float3 p1 = capl.m_Segment.m_Start;
	float3 p2 = capl.m_Segment.m_End;
	float3 p3 = capr.m_Segment.m_Start;
	float3 p4 = capr.m_Segment.m_End;

	float d1321 = (p1.x - p3.x)*(p2.x - p1.x) + (p1.y - p3.y)*(p2.y - p1.y) + (p1.z - p3.z)*(p2.z - p1.z);
	float d2121 = (p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y) + (p2.z - p1.z)*(p2.z - p1.z);
	float d4321 = (p4.x - p3.x)*(p2.x - p1.x) + (p4.y - p3.y)*(p2.y - p1.y) + (p4.z - p3.z)*(p2.z - p1.z);
	float d1343 = (p1.x - p3.x)*(p4.x - p3.x) + (p1.y - p3.y)*(p4.y - p3.y) + (p1.z - p3.z)*(p4.z - p3.z);
	float d4343 = (p4.x - p3.x)*(p4.x - p3.x) + (p4.y - p3.y)*(p4.y - p3.y) + (p4.z - p3.z)*(p4.z - p3.z);

	float t = (d1343 * d4321 - d1321 * d4343) / (d2121*d4343 - d4321*d4321);
	float u = (d1343 + t * d4321) / d4343;

	t = min(max(t, 1), 0);
	u = min(max(u, 1), 0);

	cpl = p1 + (p2 - p1)*t;
	cpr = p3 + (p4 - p3)*u;

	Sphere s1;
	Sphere s2;

	s1.m_Center = cpl;
	s1.m_Radius = capl.m_Radius;
	s2.m_Center = cpr;
	s2.m_Radius = capr.m_Radius;

	return SphereToSphere(s1, s2);
	//float3 d1 = capl.m_Segment.m_End - capl.m_Segment.m_Start;
	//float3 d2 = capr.m_Segment.m_End - capr.m_Segment.m_Start;
	//
	//float a = dot_product(d1, d1);
	//float b = dot_product(d1, d2);
	//float e = dot_product(d2, d2);
	//
	//float d = a*e - b*b;
	//if (d != 0)
	//{
	//	float3 r = capl.m_Segment.m_Start - capr.m_Segment.m_Start;
	//	float c = dot_product(d1, r);
	//	float f = dot_product(d2, r);
	//
	//	float s = (b*f - c*e) / d;
	//	float t = (a*f - b*c) / d;
	//
	//	cpl = d1 * min(max(s,0),1);
	//	cpr = d2 * min(max(t,0),1);
	//
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}
	//float3 la = capl.m_Segment.m_End - capl.m_Segment.m_Start;
	//float3 lb = capr.m_Segment.m_End - capr.m_Segment.m_Start;
	//
	//float lala = dot_product(la, la);
	//float lblb = dot_product(lb, lb);
	//float lalb = dot_product(la, lb);
	//float lapba = dot_product(la, capr.m_Segment.m_Start - capl.m_Segment.m_Start);
	//float lbpba = dot_product(lb, capr.m_Segment.m_Start - capl.m_Segment.m_Start);
	//
	//float s = (lapba + lalb*t) / lala;
	//float t = (lbpba + lalb*((lapba + lalb*t) / lala)) / lblb;
	//float3 r = capl.m_Segment.m_End - capl.m_Segment.m_Start;
	//float3 s = capr.m_Segment.m_End - capr.m_Segment.m_Start;
	//float3 qp = (capr.m_Segment.m_Start - capl.m_Segment.m_Start);
	//
	//float3 rxs;
	//cross_product(rxs,r, s);
	//float3 qpxr;
	//cross_product(qpxr, capl.m_Segment.m_Start - capr.m_Segment.m_Start, r);
	//
	//float3 t;
	//cross_product(t, qp, s);
	//
	//t = t / rxs;
	//
	//float3 u;
	//cross_product(u, qp, r);
	//u = u / rxs;
	//
	//cpl = capl.m_Segment.m_Start + t*r;
}

float SweptAABBtoAABB(const AABB& boxl, const AABB& boxr, float3 vel, float& normalx, float& normaly, float& normalz)
{
	float xClose, yClose, zClose;
	float xFar, yFar, zFar;
	normalx = normaly = normalz = 0;

	if (vel.x > 0.0f)
	{
		xClose = boxr.min.x - boxl.max.x;
		xFar = boxr.max.x - boxl.min.x;
	}
	else
	{
		xClose = boxr.max.x - boxl.min.x;
		xFar = boxr.min.x - boxl.max.x;
	}
	if (vel.y > 0.0f)
	{
		yClose = boxr.min.y - boxl.max.y;
		yFar = boxr.max.y - boxl.min.y;
	}
	else
	{
		yClose = boxr.max.y - boxl.min.y;
		yFar = boxr.min.y - boxl.max.y;
	}
	if (vel.z > 0.0f)
	{
		zClose = boxr.min.z - boxl.max.z;
		zFar = boxr.max.z - boxl.min.z;
	}
	else
	{
		zClose = boxr.max.z - boxl.min.z;
		zFar = boxr.min.z - boxl.max.z;
	}

	float xEntry, yEntry, zEntry;
	float xExit, yExit, zExit;

	if (vel.x == 0)
	{
		xEntry = -FLT_MAX;
		xExit = FLT_MAX;
	}
	else
	{
		xEntry = xClose / vel.x;
		xExit = xFar / vel.x;
	}

	if (vel.y == 0)
	{
		yEntry = -FLT_MAX;
		yExit = FLT_MAX;
	}
	else
	{
		yEntry = yClose / vel.y;
		yExit = yFar / vel.y;
	}

	if (vel.z == 0)
	{
		zEntry = -FLT_MAX;
		zExit = FLT_MAX;
	}
	else
	{
		zEntry = zClose / vel.z;
		zExit = zFar / vel.z;
	}

	float entryT = max(xEntry, max(yEntry, zEntry));
	float exitT = min(xExit, min(yExit, zExit));

	if (entryT > exitT || xEntry < 0.0f && yEntry < 0.0f && zEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f || zEntry > 1.0f)
	{
		return 1.0f;
	}

	if (xEntry > yEntry && xEntry > zEntry)
	{
		if (xClose < 0.0f)
		{
			normalx = 1.0f;
			normaly = 0.0f;
			normalz = 0.0f;
		}
		else
		{
			normalx = -1.0f;
			normaly = 0.0f;
			normalz = 0.0f;
		}
	}
	else if (yEntry > zEntry)
	{
		if (yClose < 0.0f)
		{
			normalx = 0.0f;
			normaly = 1.0f;
			normalz = 0.0f;
		}
		else
		{
			normalx = 0.0f;
			normaly = -1.0f;
			normalz = 0.0f;
		}
	}
	else
	{
		if (zClose < 0.0f)
		{
			normalx = 0.0f;
			normaly = 0.0f;
			normalz = 1.0f;
		}
		else
		{
			normalx = 0.0f;
			normaly = 0.0f;
			normalz = -1.0f;
		}
	}

	return entryT;
}

float3 SweptSpheretoAABB(Sphere& s, const AABB& b, float3& vel)
{
	float3 centerb = (b.max + b.min) * 0.5f;
	float3 move = (s.m_Center - centerb).normalize();
	AABB offset;
	offset.max = move * s.m_Radius + b.max;
	offset.min = move * s.m_Radius + b.min;
	float3 v1(offset.min.x, offset.max.y, offset.max.z);
	float3 v7(offset.max.x, offset.min.y, offset.min.z);

	float3 y(0, 1, 0);
	float3 ny(0, -1, 0);
	float3 x(1, 0, 0);
	float3 nx(-1, 0, 0);
	float3 z(0, 0, 1);
	float3 nz(0, 0, -1);

	//test y
	float3 l = (s.m_Center + vel) - s.m_Center;
	float3 testyznx = v1 - s.m_Center;
	float3 testyznxl = v1 - (s.m_Center + l);
	float3 testnynzx = v7 - s.m_Center;
	float3 testnynzxl = v7 - (s.m_Center + l);
	float doty = dot_product(testyznx, y);
	float dotyl = dot_product(testyznxl, y);
	float dotny = dot_product(testnynzx, ny);
	float dotnyl = dot_product(testnynzxl, ny);
	if (doty <= 0 && dotyl >= 0)
	{
		float d0 = dot_product(y, s.m_Center);
		float d1 = dot_product(y, v1);
		float d2 = d0 - d1;
		float d3 = dot_product(y, l);
		float df = -(d2 / d3);
		float3 cp = s.m_Center + l * df;
		if (cp.x >= offset.min.x && cp.x <= offset.max.x && cp.z >= offset.min.z && cp.z <= offset.max.z)
		{
			s.m_Center = cp + float3(0, 0.0001f, 0);
			return float3(1, -1, 1);
		}
	}
	else if (dotny <= 0 && dotnyl >= 0)
	{
		float d0 = dot_product(ny, s.m_Center);
		float d1 = dot_product(ny, v7);
		float d2 = d0 - d1;
		float d3 = dot_product(ny, l);
		float df = -(d2 / d3);
		float3 cp = s.m_Center + l * df;
		if (cp.x >= offset.min.x && cp.x <= offset.max.x && cp.z >= offset.min.z && cp.z <= offset.max.z)
		{
			s.m_Center = cp + float3(0, -0.0001f, 0);
			return float3(1, -1, 1);
		}
	}

	float dotx = dot_product(testnynzx, x);
	float dotxl = dot_product(testnynzxl, x);
	float dotnx = dot_product(testyznx, nx);
	float dotnxl = dot_product(testyznxl, nx);
	if (dotx <= 0 && dotxl >= 0)
	{
		float d0 = dot_product(x, s.m_Center);
		float d1 = dot_product(x, v7);
		float d2 = d0 - d1;
		float d3 = dot_product(x, l);
		float df = -(d2 / d3);
		float3 cp = s.m_Center + l * df;
		if (cp.y >= offset.min.y && cp.y <= offset.max.y && cp.z >= offset.min.z && cp.z <= offset.max.z)
		{
			s.m_Center = cp + float3(0.0001f, 0, 0);
			return float3(-1, 1, 1);
		}
	}
	else if (dotnx <= 0 && dotnxl >= 0)
	{
		float d0 = dot_product(nx, s.m_Center);
		float d1 = dot_product(nx, v1);
		float d2 = d0 - d1;
		float d3 = dot_product(nx, l);
		float df = -(d2 / d3);
		float3 cp = s.m_Center + l * df;
		if (cp.y >= offset.min.y && cp.y <= offset.max.y && cp.z >= offset.min.z && cp.z <= offset.max.z)
		{
			s.m_Center = cp + float3(-0.0001f, 0, 0);
			return float3(-1, 1, 1);
		}
	}

	float dotz = dot_product(testyznx, z);
	float dotzl = dot_product(testyznxl, z);
	float dotnz = dot_product(testnynzx, nz);
	float dotnzl = dot_product(testnynzxl, nz);
	if (dotz <= 0 && dotzl >= 0)
	{
		float d0 = dot_product(z, s.m_Center);
		float d1 = dot_product(z, v1);
		float d2 = d0 - d1;
		float d3 = dot_product(z, l);
		float df = -(d2 / d3);
		float3 cp = s.m_Center + l * df;
		if (cp.x >= offset.min.x && cp.x <= offset.max.x && cp.y >= offset.min.y && cp.y <= offset.max.y)
		{
			s.m_Center = cp + float3(0, 0, 0.0001f);
			return float3(1, 1, -1);
		}
	}
	else if (dotnz <= 0 && dotnzl >= 0)
	{
		float d0 = dot_product(nz, s.m_Center);
		float d1 = dot_product(nz, v7);
		float d2 = d0 - d1;
		float d3 = dot_product(nz, l);
		float df = -(d2 / d3);
		float3 cp = s.m_Center + l * df;
		if (cp.x >= offset.min.x && cp.x <= offset.max.x && cp.y >= offset.min.y && cp.y <= offset.max.y)
		{
			s.m_Center = cp + float3(0, 0, -0.0001f);
			return float3(1, 1, -1);
		}
	}

	else if (dotx <= 0 && dotnx <= 0 && doty <= 0 && dotny <= 0 && dotz <= 0 && dotnz <= 0)
	{
		float3 cmax = offset.max - s.m_Center;
		float3 cmin = s.m_Center - offset.min;
		if (cmax.x < cmin.x && cmax.x < cmax.y && cmax.x < cmin.y && cmax.x < cmax.z && cmax.x < cmin.z)
		{
			s.m_Center.x = cmax.x + s.m_Radius;
			return float3(-1, 1, 1);
		}
		else if (cmax.x > cmin.x && cmin.x < cmax.y && cmin.x < cmin.y && cmin.x < cmax.z && cmin.x < cmin.z)
		{
			s.m_Center.x = cmin.x - s.m_Radius;
			return float3(-1, 1, 1);
		}
		else if (cmax.y < cmin.x && cmax.y < cmax.x && cmax.y < cmin.y && cmax.y < cmax.z && cmax.y < cmin.z)
		{
			s.m_Center.y = cmax.y + s.m_Radius;
			return float3(1, -1, 1);
		}
		else if (cmax.x > cmin.y && cmin.y < cmax.y && cmin.y < cmin.x && cmin.y < cmax.z && cmin.y < cmin.z)
		{
			s.m_Center.y = cmin.y - s.m_Radius;
			return float3(1, -1, 1);
		}
		else if (cmax.z < cmin.x && cmax.z < cmax.y && cmax.z < cmin.y && cmax.z < cmax.x && cmax.z < cmin.z)
		{
			s.m_Center.z = cmax.z + s.m_Radius;
			return float3(1, 1, -1);
		}
		else if (cmax.x > cmin.z && cmin.z < cmax.y && cmin.z < cmin.y && cmin.z < cmax.z && cmin.z < cmin.x)
		{
			s.m_Center.z = cmax.z - s.m_Radius;
			return float3(1, 1, -1);
		}
	}


	return float3().make_zero();
}

float3 SweptSpheretoSphere(Sphere& sp, const Sphere& ss, float3& vel)
{
	float3 s = sp.m_Center;
	float3 e = sp.m_Center + vel;
	float3 se = e - s;
	float ratio = dot_product(se, ss.m_Center - s) / dot_product(se, se);
	ratio = max(0, min(ratio, 1));
	se = s + se * ratio;
	if (dot_product(ss.m_Center - se, ss.m_Center - se) > powf(ss.m_Radius + sp.m_Radius, 2))
	{
		return float3().make_zero();
	}
	sp.m_Center = se;
	float3 n = (sp.m_Center - ss.m_Center).normalize();
	vel = vel - n * 2 * dot_product(vel, n);
	return n;
}

bool SweptSpheretoSweptSphere(Sphere& sl, Sphere& sr, float3& vell, float3& velr)
{
	float3 va = vell;
	float3 vb = velr;
	float3 ab = sr.m_Center - sl.m_Center;
	float3 vab = vb - va;
	float rab = sl.m_Radius + sr.m_Radius;
	float a = dot_product(vab, vab) + 0.0001f;
	float b = 2 * dot_product(vab, ab) + 0.0001f;
	float c = dot_product(ab, ab) - rab * rab + 0.0001f;
	float u0 = 0;
	float u1 = 0;
	if (dot_product(ab, ab) <= rab*rab)
	{
		float3 n = (sr.m_Center - sl.m_Center).normalize();
		vell = vell - n * 2 * dot_product(vell, n);
		velr = velr - n.negate() * 2 * dot_product(velr, n);
	}
	else if (QuadraticFormula(a, b, c, u0, u1))
	{
		if (u0 > u1)
		{
			u0 += u1;
			u1 = u0 - u1;
			u0 -= u1;
		}
		sl.m_Center = sl.m_Center + vell * u0;
		sr.m_Center = sr.m_Center + velr * u0;
		float3 nl = (sr.m_Center - sl.m_Center).normalize();
		float3 nr = (sl.m_Center - sr.m_Center).normalize();
		vell = vell - nl * 2 * dot_product(vell, nl);
		velr = velr - nr * 2 * dot_product(velr, nr);
	}
	if (u0 <= 0 || u0 >= 1)
		return false;
	return true;
}

bool stuff(Capsule& capl, Capsule& capr, float3& vell, float3& velr, float3& pos, float3& opos)
{
	float3 cpl;
	float3 cpr;

	float3 p1 = capl.m_Segment.m_Start;
	float3 p2 = capl.m_Segment.m_End;
	float3 p3 = capr.m_Segment.m_Start;
	float3 p4 = capr.m_Segment.m_End;

	float d1321 = (p1.x - p3.x)*(p2.x - p1.x) + (p1.y - p3.y)*(p2.y - p1.y) + (p1.z - p3.z)*(p2.z - p1.z);
	float d2121 = (p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y) + (p2.z - p1.z)*(p2.z - p1.z);
	float d4321 = (p4.x - p3.x)*(p2.x - p1.x) + (p4.y - p3.y)*(p2.y - p1.y) + (p4.z - p3.z)*(p2.z - p1.z);
	float d1343 = (p1.x - p3.x)*(p4.x - p3.x) + (p1.y - p3.y)*(p4.y - p3.y) + (p1.z - p3.z)*(p4.z - p3.z);
	float d4343 = (p4.x - p3.x)*(p4.x - p3.x) + (p4.y - p3.y)*(p4.y - p3.y) + (p4.z - p3.z)*(p4.z - p3.z);

	float t = (d1343 * d4321 - d1321 * d4343) / (d2121*d4343 - d4321*d4321);
	float u = (d1343 + t * d4321) / d4343;

	t = min(max(t, 1), 0);
	u = min(max(u, 1), 0);

	cpl = p1 + (p2 - p1)*t;
	cpr = p3 + (p4 - p3)*u;

	Sphere s1;
	Sphere s2;

	s1.m_Center = cpl;
	s1.m_Radius = capl.m_Radius;
	s2.m_Center = cpr;
	s2.m_Radius = capr.m_Radius;

	if (SphereToSphere(s1, s2))
	{
		float3 mid = (cpl + cpr) * 0.5f;
		float3 ml = mid + (cpl - mid).normalize() * (capl.m_Radius + 0.0001f);
		float3 mr = mid + (cpr - mid).normalize() * (capr.m_Radius + 0.0001f);

		pos = ml;
		opos = mr;

		float3 nl = (opos - pos).normalize();
		float3 nr = (pos - opos).normalize();
		vell = vell - nr * 2 * dot_product(vell, nr);
		velr = velr - nl * 2 * dot_product(velr, nl);

		return true;
	}

	return false;
}

bool SweptCaptoSweptCap(Capsule& capl, Capsule& capr, float3& vell, float3& velr, float3& pos, float3& opos)
{
	//float3 p0 = capl.m_Segment.m_Start;
	//float3 u = capl.m_Segment.m_End - capl.m_Segment.m_Start;
	//
	//float3 q0 = capr.m_Segment.m_Start;
	//float3 v = capr.m_Segment.m_End - capr.m_Segment.m_Start;
	//float3 w0 = p0 - q0;
	//
	//float a = dot_product(u, u);
	//float b = dot_product(u, v) + 0.00001f;
	//float c = dot_product(v, v);
	//float d = dot_product(u, w0);
	//float e = dot_product(v, w0);
	//
	//float sc = (b*e - c*d) / (a*c - b*b);
	//float tc = (a*e - b*d) / (a*c - b*b);
	//
	//sc = max(0, min(sc, 1));
	//tc = max(0, min(tc, 1));
	//
	//p0 = p0 + u*sc;
	//q0 = q0 + v*tc;

	AABB bl;
	AABB br;
	bl.min = capl.m_Segment.m_Start - float3(capl.m_Radius + capl.m_Radius, capl.m_Radius + capl.m_Radius, capl.m_Radius + capl.m_Radius);
	bl.max = capl.m_Segment.m_End + float3(capl.m_Radius + capl.m_Radius, capl.m_Radius + capl.m_Radius, capl.m_Radius + capl.m_Radius);

	br.min = capr.m_Segment.m_Start - float3(capr.m_Radius + capr.m_Radius, capr.m_Radius + capr.m_Radius, capr.m_Radius + capr.m_Radius);
	br.max = capr.m_Segment.m_End + float3(capr.m_Radius + capr.m_Radius, capr.m_Radius + capr.m_Radius, capr.m_Radius + capr.m_Radius);

	if (!AABBtoAABB(bl, br))
		return false;


	float3 cpol = capl.m_Segment.m_Start;
	cpol.y = min(max(capl.m_Segment.m_Start.y, capr.m_Segment.m_Start.y), capl.m_Segment.m_End.y);
	float3 cpor = capr.m_Segment.m_Start;
	cpor.y = min(max(capl.m_Segment.m_Start.y, capr.m_Segment.m_Start.y), capr.m_Segment.m_End.y);

	Sphere sl;
	Sphere sr;
	sl.m_Center = cpol;
	sr.m_Center = cpor;
	sl.m_Radius = capl.m_Radius;
	sr.m_Radius = capr.m_Radius;
	if (SphereToSphere(sl, sr))
	{
		float3 ml = (cpor - cpol).normalize() * (capl.m_Radius);
		float3 mr = (cpol - cpor).normalize() * (capr.m_Radius);
		float3 mid = (cpol + ml + cpor + mr) * 0.5f;

		ml = mid + ((cpol - mid).normalize() * (capl.m_Radius));
		mr = mid + ((cpor - mid).normalize() * (capr.m_Radius));

		pos = float3(0, capl.m_Segment.m_Start.y - ml.y, 0) + ml;
		opos = float3(0, capr.m_Segment.m_Start.y - mr.y, 0) + mr;

		return true;
	}
	return false;

	/*float3 p0;
	float3 q0;

	float3 p1 = capl.m_Segment.m_Start;
	float3 p2 = capl.m_Segment.m_End;
	float3 p3 = capr.m_Segment.m_Start;
	float3 p4 = capr.m_Segment.m_End;
	float3 pr;
	float3 pl;
	float dot = dot_product((p2 - p1).normalize(), (p4 - p3).normalize());
	float t;
	float u;
	if (dot == 1 || dot == -1)
	{
		pr.x = max(min(max(p3.x, p4.x), p1.x), min(p3.x, p4.x));
		pr.y = max(min(max(p3.y, p4.y), p1.y), min(p3.y, p4.y));
		pr.z = max(min(max(p3.z, p4.z), p1.z), min(p3.z, p4.z));

		pl.x = max(min(max(p1.x, p2.x), p3.x), min(p1.x, p2.x));
		pl.y = max(min(max(p1.y, p2.y), p3.y), min(p1.y, p2.y));
		pl.z = max(min(max(p1.z, p2.z), p3.z), min(p1.z, p2.z));
	}
	else
	{
		float d1321 = (p1.x - p3.x)*(p2.x - p1.x) + (p1.y - p3.y)*(p2.y - p1.y) + (p1.z - p3.z)*(p2.z - p1.z);
		float d2121 = (p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y) + (p2.z - p1.z)*(p2.z - p1.z);
		float d4321 = (p4.x - p3.x)*(p2.x - p1.x) + (p4.y - p3.y)*(p2.y - p1.y) + (p4.z - p3.z)*(p2.z - p1.z);
		float d1343 = (p1.x - p3.x)*(p4.x - p3.x) + (p1.y - p3.y)*(p4.y - p3.y) + (p1.z - p3.z)*(p4.z - p3.z);
		float d4343 = (p4.x - p3.x)*(p4.x - p3.x) + (p4.y - p3.y)*(p4.y - p3.y) + (p4.z - p3.z)*(p4.z - p3.z);

		t = (d1343 * d4321 - d1321 * d4343) / (d2121*d4343 - d4321*d4321 + 0.00001f);
		u = (d1343 + t * d4321) / d4343;

		t = min(max(t, 1), 0);
		u = min(max(u, 1), 0);

		pl = p1 + (p2 - p1)*t;
		pr = p3 + (p4 - p3)*u;
	}
	p0 = pl;
	q0 = pr;
	Sphere sl;
	Sphere sr;
	sl.m_Center = p0;
	sr.m_Center = q0;
	sl.m_Radius = capl.m_Radius;
	sr.m_Radius = capr.m_Radius;
	if (SphereToSphere(sl, sr))
	{
		float3 ml = (q0 - p0).normalize() * (capl.m_Radius + 0.00011f);
		float3 mr = (p0 - q0).normalize() * (capr.m_Radius + 0.00011f);
		float3 mid = (p0 + ml + q0 + mr) * 0.5f;

		ml += (p0 - mid).normalize() * (capl.m_Radius + 0.0011f);
		mr += (q0 - mid).normalize() * (capr.m_Radius + 0.0011f);

		pos += ml;
		opos += mr;

		float3 nl = (q0 - p0).normalize();
		float3 nr = (p0 - q0).normalize();
		vell = vell - nr * 2 * dot_product(vell, nr);
		velr = velr - nl * 2 * dot_product(velr, nl);
		return true;
	}
	return false;*/
}

float3 AABBToCapsuleReact(const AABB& box, Capsule& cap, float3& vel, float3& pos)
{
	float3 Center = (box.min + box.max) * 0.5f;
	vec3f cp = cap.m_Segment.m_End - cap.m_Segment.m_Start;
	float ratio = dot_product(cp, Center - cap.m_Segment.m_Start) / dot_product(cp, cp);
	ratio = max(min(ratio, 1), 0);
	cp = cap.m_Segment.m_Start + cp * ratio;
	vec3f cpb = cp;
	if (cp.x < box.min.x)
		cpb.x = box.min.x;
	if (cp.y < box.min.y)
		cpb.y = box.min.y;
	if (cp.z < box.min.z)
		cpb.z = box.min.z;
	if (cp.x > box.max.x)
		cpb.x = box.max.x;
	if (cp.y > box.max.y)
		cpb.y = box.max.y;
	if (cp.z > box.max.z)
		cpb.z = box.max.z;
	if (dot_product(cp - cpb, cp - cpb) < powf(cap.m_Radius, 2))
	{
		float3 v1(box.min.x, box.max.y, box.max.z);
		float3 v7(box.max.x, box.min.y, box.min.z);
		cpb = cp;
		float3 y(0, 1, 0);
		float3 ny(0, -1, 0);
		float3 x(1, 0, 0);
		float3 nx(-1, 0, 0);
		float3 z(0, 0, 1);
		float3 nz(0, 0, -1);

		float3 testyznx = v1 - cp;
		float3 testnynzx = v7 - cp;

		float dx = dot_product(testnynzx, x);
		float dnx = dot_product(testyznx, nx);
		float dy = dot_product(testyznx, y);
		float dny = dot_product(testnynzx, ny);
		float dz = dot_product(testyznx, z);
		float dnz = dot_product(testnynzx, nz);
		float3 ref;
		if (dx < dnx && dx < dy && dx < dny && dx < dz && dx < dnz)
		{
			cp.x = box.max.x + cap.m_Radius - 0.000001f;
			ref = x;
		}
		else if (dnx < dx && dnx < dy && dnx < dny && dnx < dz && dnx < dnz)
		{
			cp.x = box.min.x - cap.m_Radius + 0.000001f;
			ref = nx;
		}
		else if (dy < dnx && dy < dx && dy < dny && dy < dz && dy < dnz)
		{
			cp.y = box.max.y + cap.m_Radius - 0.000001f;
			ref = y;
		}
		else if (dny < dnx && dny < dy && dny < dx && dny < dz && dny < dnz)
		{
			cp.y = box.min.y - cap.m_Radius + 0.000001f;
			ref = ny;
		}
		else if (dz < dnx && dz < dy && dz < dny && dz < dx && dz < dnz)
		{
			cp.z = box.max.z + cap.m_Radius - 0.000001f;
			ref = z;
		}
		else if (dnz < dnx && dnz < dy && dnz < dny && dnz < dz && dnz < dx)
		{
			cp.z = box.min.z - cap.m_Radius + 0.000001f;
			ref = nz;
		}
		else
		{
			pos.y = box.max.y;
		}
		pos = cp;
		return ref;
	}
	return float3(0, 0, 0);
}

bool CapsuleToSphereReact(const Capsule& capsule, Sphere& sphere, float3& vel)
{
	vec3f SE = capsule.m_Segment.m_End - capsule.m_Segment.m_Start;
	float ratio = dot_product(SE, sphere.m_Center - capsule.m_Segment.m_Start) / dot_product(SE, SE);
	ratio = max(0, min(ratio, 1));
	SE = capsule.m_Segment.m_Start + SE * ratio;
	if (dot_product(sphere.m_Center - SE, sphere.m_Center - SE) > powf(sphere.m_Radius + capsule.m_Radius, 2))
		return false;
	float3 cs = sphere.m_Center - SE;
	float3 ref = cs.normalize();
	float3 m = ref * (sphere.m_Radius + capsule.m_Radius);
	sphere.m_Center += m;
	vel = vel - ref * 2 * dot_product(vel, ref);
	return true;
}

bool OldHexagonToSphere(const Hexagon& hex, Sphere& s, float3& vel)
{
	AABB bounding;
	bounding.min = float3(hex.seg.m_Start.x - 2, hex.seg.m_Start.y - 1, hex.seg.m_Start.z - 3);
	bounding.max = float3(hex.seg.m_End.x + 2, hex.seg.m_End.y + 1, hex.seg.m_End.z + 3);

	if (!SphereToAABB(s, bounding))
		return false;

	vec3f SE = hex.seg.m_End - hex.seg.m_Start;
	float ratio = dot_product(SE, s.m_Center - hex.seg.m_Start) / dot_product(SE, SE);
	ratio = max(0, min(ratio, 1));
	SE = hex.seg.m_Start + SE * ratio;


	float3 sc = SE - float3(abs(s.m_Center.x), s.m_Center.y, abs(s.m_Center.z));
	sc = sc.normalize();
	sc = s.m_Center + sc * s.m_Radius;

	float hh = hex.h * 0.5f;

	AABB top;
	AABB bottom;
	top.min = float3(hex.seg.m_End.x - hh * 0.9f, hex.seg.m_End.y - 0.33f, hex.seg.m_End.z - hex.s * 0.4f);
	top.max = float3(hex.seg.m_End.x + hh * 0.9f, hex.seg.m_End.y + 0.0001f, hex.seg.m_End.z + hex.s * 0.4f);

	bottom.min = float3(hex.seg.m_Start.x - hh * 0.9f, hex.seg.m_Start.y - 0.0001f, hex.seg.m_Start.z - hex.s * 0.4f);
	bottom.max = float3(hex.seg.m_Start.x + hh * 0.9f, hex.seg.m_Start.y + 0.33f, hex.seg.m_Start.z + hex.s * 0.4f);

	float3 tc = SweptSpheretoAABB(s, top, vel);
	if (!tc.isEquil(float3(0, 0, 0)))
	{
		vel *= tc;
		return true;
	}
	float3 bc = SweptSpheretoAABB(s, bottom, vel);
	if (!bc.isEquil(float3(0, 0, 0)))
	{
		vel *= bc;
		return true;
	}

	float q2x = abs(sc.x - SE.x);
	float q2z = abs(sc.z - SE.z);
	if (q2x > hh || q2z > hex.s) return false;
	if (sc.y > hex.seg.m_End.y || sc.y < hex.seg.m_Start.y) return false;

	float3 velc = vel;
	vel = float3(abs(vel.x), abs(vel.y), abs(vel.z));

	float3 n = (sc - SE).normalize();
	vel = vel - n * 2 * dot_product(vel, n);

	//pastPos *= 0.04f;

	if (velc.x < 0) vel.x *= -1;
	if (velc.y < 0) vel.y *= -1;
	if (velc.z < 0) vel.z *= -1;
	return hex.s * hh - (hex.s * 0.5f) * q2x - hh * q2z >= 0;
}

float3 SpherePlanePoint(const NewPlane& p, const Sphere& s)
{
	float3 v = s.m_Center - p.p;
	float d = dot_product(v, p.n);
	float3 np = p.n * (s.m_Radius - d);
	return s.m_Center + np;
}

bool IntersectRayTriangle(const vec3f &vert0, const vec3f &vert1, const vec3f &vert2, const vec3f &norm, const vec3f &start, const vec3f &d, float &t)
{
	vec3f dirtopoint = start - vert0;
	if (dot_product(dirtopoint, norm) < 0) return false;
	if (dot_product(norm, d) > 0) return false;

	vec3f s1 = vert0 - start;
	vec3f s2 = vert1 - start;
	vec3f s3 = vert2 - start;
	vec3f sa;
	vec3f sb;
	vec3f sc;
	cross_product(sa, s3, s2);
	cross_product(sb, s1, s3);
	cross_product(sc, s2, s1);
	float n1 = dot_product(sa, d);
	float n2 = dot_product(sb, d);
	float n3 = dot_product(sc, d);
	if (SameSign(n1, n2) && SameSign(n1, n3) && SameSign(n2, n3))
	{
		float D = dot_product(norm, vert0);
		t = (D - dot_product(start, norm)) / dot_product(norm, d);
		return true;
	}
	return false;
}

bool IntersectRaySphere(const vec3f &p, const vec3f &d, const vec3f &center, float radius, float &t, vec3f &q)
{
	t = FLT_MAX;
	vec3f dirP = p - center;
	//if (dot_product(d, dirP) < 0) return false;
	float b = dot_product(dirP, d);
	float c = dot_product(dirP, dirP) - radius * radius;
	if (c > 0 && b > 0) return false;
	float discr = b*b - c;
	if (discr < 0) return false;
	t = -b - sqrt(discr);
	if (t < 0) t = 0;
	q = p + (d * t);
	return true;
}

bool IntersectRayCylinder(const vec3f &sa, const vec3f &n, const vec3f &p, const vec3f &q, float r, float &t)
{
	vec3f d = q - p; // vector from first point on cylinder segment to the end point on cylinder segment
	vec3f m = sa - p; // vector from first point on cylinder segment to start point of ray

					  // Values used to calculate coefficients of quadratic formula.
					  // You do not necessarily have to use any of these directly for the rest of the algorithm.
	float dd = dot_product(d, d); // dot product of d with d (squared magnitude of d)
	float nd = dot_product(n, d); // dot product of ray normal (n) with d
	float mn = dot_product(m, n);
	float md = dot_product(m, d);
	float mm = dot_product(m, m);

	//		 If the ray starts outside the top or bottom planes and points away, there can be no intersection.
	if (dot_product(sa - p, d) < 0 && dot_product(d, n) < 0) return false;
	if (dot_product(sa - q, d) > 0 && dot_product(d, n) > 0) return false;

	// Coefficients for the quadratic formula
	float a = dd - nd * nd;
	float b = dd*mn - nd*md;
	float c = dd*(mm - r*r) - md*md;

	// If a is approximately 0.0 then the ray is parallel to the cylinder and can't intersect
	if (abs(a) < FLT_EPSILON)
		return false;
	t = FLT_MAX;
	//		 Use the quadratic formula to solve for t. Reference "Advanced Ray to Sphere.ppt" for an example.
	//		 As with "Advanced Ray to Sphere", the 2s and 4 in the formula ( x = (-b - sqrt(b*b - 4ac)) / 2a )
	//		 are cancelled out, resulting in a simplified form.
	if (b*b - a*c < 0) return false;
	t = (-b - sqrt(b*b - a *c)) / a;
	if (t < 0)
	{
		t = FLT_MAX;
		return false;
	}
	vec3f cp = sa + n * t;
	if (dot_product(cp - p, d) < 0)
		return false;
	if (dot_product(cp - q, d) > 0)
		return false;
	return true;
}

bool IntersectRayCapsule(const vec3f &sa, const vec3f &n, const vec3f &p, const vec3f &q, float r, float &t)
{
	float fTime = FLT_MAX;
	t = FLT_MAX;
	bool bReturn = false;

	if (IntersectRayCylinder(sa, n, p, q, r, t))
		return true;
	vec3f point;
	if (IntersectRaySphere(sa, n, p, r, fTime, point))
		bReturn = true;
	if (IntersectRaySphere(sa, n, q, r, t, point))
		bReturn = true;
	if (fTime < t) t = fTime;
	return bReturn;
}

bool IntersectMovingSphereTriangle(const vec3f &vert0, const vec3f &vert1, const vec3f &vert2, const vec3f &norm, const vec3f &start, const vec3f &d, float r, float &t, vec3f &outNormal)
{
	bool bReturn = false;
	float fTime = FLT_MAX;
	t = FLT_MAX;

	vec3f verts[3] = { vert0, vert1, vert2 };
	verts[0] += norm * r;
	verts[1] += norm * r;
	verts[2] += norm * r;

	if (IntersectRayTriangle(verts[0], verts[1], verts[2], norm, start, d, t))
	{
		outNormal = norm;
		return true;
	}
	vec3f l;
	vec3f n;
	vec3f v;
	float dis;
	vec3f cp;
	if (IntersectRayCapsule(start, d, vert0, vert1, r, t))
	{
		l = vert1 - vert0;
		n = l.normalize();
		cp = start + d * t;
		v = cp - vert0;
		dis = dot_product(n, v);
		n = n * dis;
		outNormal = (cp - (vert0 + n)).normalize();
		return true;
	}
	if (IntersectRayCapsule(start, d, vert1, vert2, r, t))
	{
		l = vert2 - vert1;
		n = l.normalize();
		cp = start + d * t;
		v = cp - vert1;
		dis = dot_product(n, v);
		n = n * dis;
		outNormal = (cp - (vert1 + n)).normalize();
		return true;
	}
	if (IntersectRayCapsule(start, d, vert2, vert0, r, t))
	{
		l = vert0 - vert2;
		n = l.normalize();
		cp = start + d * t;
		v = cp - vert2;
		dis = dot_product(n, v);
		n = n * dis;
		outNormal = (cp - (vert2 + n)).normalize();
		return true;
	}
	return false;
}

struct Triangle
{
	float3* p0 = nullptr;
	float3* p1 = nullptr;
	float3* p2 = nullptr;
	float3* n = nullptr;
	void SetTriangle(float3* _p0, float3* _p1, float3* _p2, float3* _n) { p0 = _p0; p1 = _p1; p2 = _p2; n = _n; };
};

bool SphereToTriangle(const Sphere& sphere, const Triangle& tri)
{

	vec3f ppt = sphere.m_Center - (*tri.n * (dot_product(sphere.m_Center - *tri.p0, *tri.n)));
	if (dot_product(sphere.m_Center - ppt, sphere.m_Center - ppt) > sphere.m_Radius * sphere.m_Radius) return false;
	vec3f edge0 = *tri.p1 - *tri.p0;
	vec3f edge1 = *tri.p2 - *tri.p1;
	vec3f edge2 = *tri.p0 - *tri.p2;
	vec3f normal0;
	cross_product(normal0, edge0, *tri.n);
	vec3f normal1;
	cross_product(normal1, edge1, *tri.n);
	vec3f normal2;
	cross_product(normal2, edge2, *tri.n);
	bool test = true;
	vec3f p0 = ppt - *tri.p0;
	if (dot_product(normal0, p0) > 0)
	{
		test = false;
	}
	vec3f p1 = ppt - *tri.p1;
	if (dot_product(normal1, p1) > 0)
	{
		test = false;
	}
	vec3f p2 = ppt - *tri.p2;
	if (dot_product(normal2, p2) > 0)
	{
		test = false;
	}
	if (!test)
	{
		float d0 = dot_product(sphere.m_Center - *tri.p0, edge0);
		float d1 = dot_product(sphere.m_Center - *tri.p1, edge1);
		float d2 = dot_product(sphere.m_Center - *tri.p2, edge2);
		d0 = clamp(d0);
		d1 = clamp(d1);
		d2 = clamp(d2);
		vec3f cp0 = *tri.p0 + edge0 * d0;
		vec3f cp1 = *tri.p1 + edge1 * d1;
		vec3f cp2 = *tri.p2 + edge2 * d2;
		float r2 = sphere.m_Radius * sphere.m_Radius;
		if (dot_product(sphere.m_Center - cp0, sphere.m_Center - cp0) < r2) return true;
		if (dot_product(sphere.m_Center - cp1, sphere.m_Center - cp1) < r2) return true;
		if (dot_product(sphere.m_Center - cp2, sphere.m_Center - cp2) < r2) return true;
		else return false;
	}
	return true;
}

bool SphereToTriangle(const Sphere& sphere, const Triangle& tri, vec3f& displacement)
{
	vec3f ppt = sphere.m_Center - (*tri.n * (dot_product(sphere.m_Center - *tri.p0, *tri.n)));
	if (dot_product(sphere.m_Center - ppt, sphere.m_Center - ppt) > sphere.m_Radius * sphere.m_Radius) return false;
	if (dot_product(sphere.m_Center - *tri.p0, *tri.n) < 0) return false;
	vec3f edge0 = *tri.p1 - *tri.p0;
	vec3f edge1 = *tri.p2 - *tri.p1;
	vec3f edge2 = *tri.p0 - *tri.p2;
	vec3f normal0;
	cross_product(normal0, edge0, *tri.n);
	vec3f normal1;
	cross_product(normal1, edge1, *tri.n);
	vec3f normal2;
	cross_product(normal2, edge2, *tri.n);
	bool test = true;
	vec3f p0 = ppt - *tri.p0;
	if (dot_product(normal0, p0) > 0)
	{
		test = false;
	}
	vec3f p1 = ppt - *tri.p1;
	if (dot_product(normal1, p1) > 0)
	{
		test = false;
	}
	vec3f p2 = ppt - *tri.p2;
	if (dot_product(normal2, p2) > 0)
	{
		test = false;
	}
	if (!test)
	{

		float d0 = dot_product(sphere.m_Center - *tri.p0, edge0);
		float d1 = dot_product(sphere.m_Center - *tri.p1, edge1);
		float d2 = dot_product(sphere.m_Center - *tri.p2, edge2);
		d0 = clamp(d0);
		d1 = clamp(d1);
		d2 = clamp(d2);
		vec3f cp0 = *tri.p0 + edge0 * d0;
		vec3f cp1 = *tri.p1 + edge1 * d1;
		vec3f cp2 = *tri.p2 + edge2 * d2;
		float r2 = sphere.m_Radius * sphere.m_Radius;
		if (dot_product(sphere.m_Center - cp0, sphere.m_Center - cp0) < r2)
		{
			vec3f v = sphere.m_Center - cp0;
			float dist = v.magnitude();
			vec3f n = v / dist;
			displacement = n * (sphere.m_Radius - dist);
			return true;
		}
		if (dot_product(sphere.m_Center - cp1, sphere.m_Center - cp1) < r2)
		{
			vec3f v = sphere.m_Center - cp1;
			float dist = v.magnitude();
			vec3f n = v / dist;
			displacement = n * (sphere.m_Radius - dist);
			return true;
		}
		if (dot_product(sphere.m_Center - cp2, sphere.m_Center - cp2) < r2)
		{
			vec3f v = sphere.m_Center - cp2;
			float dist = v.magnitude();
			vec3f n = v / dist;
			displacement = n * (sphere.m_Radius - dist);
			return true;
		}
		else return false;
	}
	vec3f v = sphere.m_Center - ppt;
	float dist = v.magnitude();
	vec3f n = v / dist;
	displacement = n * (sphere.m_Radius - dist);
	return true;
}

bool IntersectMovingSphereMesh(const vec3f &start, const vec3f &d, float r, const ED2Mesh* mesh, float &t, vec3f &outNormal)
{
	// TODO: Read the header file comments for this function!
	//std::vector<unsigned int[3]> index;

	bool bCollision = false;
	t = FLT_MAX;
	float fTime = FLT_MAX;

	// TODO: Complete this function
	for (int i = 0; i < mesh->m_Triangles.size(); ++i)
	{
		unsigned int temp[3];
		if (mesh->m_Triangles[i].indices[1] < mesh->m_Triangles[i].indices[0])
		{
			if (mesh->m_Triangles[i].indices[2] < mesh->m_Triangles[i].indices[1])
			{
				temp[0] = mesh->m_Triangles[i].indices[2];
				temp[1] = mesh->m_Triangles[i].indices[1];
				temp[2] = mesh->m_Triangles[i].indices[0];
			}
			else
			{
				temp[0] = mesh->m_Triangles[i].indices[1];
				if (mesh->m_Triangles[i].indices[2] < mesh->m_Triangles[i].indices[0])
				{
					temp[1] = mesh->m_Triangles[i].indices[2];
					temp[2] = mesh->m_Triangles[i].indices[0];
				}
				else
				{
					temp[1] = mesh->m_Triangles[i].indices[0];
					temp[2] = mesh->m_Triangles[i].indices[2];
				}
			}
		}
		else if (mesh->m_Triangles[i].indices[2] < mesh->m_Triangles[i].indices[0])
		{
			temp[0] = mesh->m_Triangles[i].indices[2];
			temp[1] = mesh->m_Triangles[i].indices[0];
			temp[2] = mesh->m_Triangles[i].indices[1];
		}
		else
		{
			temp[0] = mesh->m_Triangles[i].indices[0];
			temp[1] = mesh->m_Triangles[i].indices[1];
			temp[2] = mesh->m_Triangles[i].indices[2];
		}
		if (IntersectMovingSphereTriangle(mesh->m_Vertices[temp[0]].pos, mesh->m_Vertices[temp[1]].pos, mesh->m_Vertices[temp[2]].pos, mesh->m_TriNorms[i], start, d, r, t, outNormal))
		{
			if (t < fTime)
				fTime = t;
			bCollision = true;
		}
	}
	t = fTime;
	return bCollision;
}

Triangle tris[18];

float3 zeroF = float3(0, 0, 0);
float3 tpln = float3(0.5f, 0, 0.866025f);
float3 rpln = float3(1.0f, 0, 0);
float3 tplnnz = float3(0.5f, 0, -0.866025f);
float3 tplnnznx = float3(-0.5f, 0, -0.866025f);
float3 rplnnx = float3(-1.0f, 0, 0);
float3 tplnnx = float3(-0.5f, 0, 0.866025f);
float3 topn = float3(0, 1, 0);

float3 top;
float3 topP0;
float3 topP1;
float3 topP2;
float3 topP3;
float3 topP4;
float3 topP5;
float3 bottom;
float3 bottomP0;
float3 bottomP1;
float3 bottomP2;
float3 bottomP3;
float3 bottomP4;
float3 bottomP5;

void BuildTris(const Hexagon& hex)
{
	top = hex.seg.m_End;
	topP0 = float3(top.x, top.y, top.z + hex.s);
	topP1 = float3(top.x + hex.h * 0.5f, top.y, top.z + hex.s * 0.5f);
	topP2 = float3(top.x + hex.h * 0.5f, top.y, top.z - hex.s * 0.5f);
	topP3 = float3(top.x, top.y, -hex.s);
	topP4 = float3(top.x - hex.h * 0.5f, top.y, top.z - hex.s * 0.5f);
	topP5 = float3(top.x - hex.h * 0.5f, top.y, top.z + hex.s * 0.5f);
	bottom = hex.seg.m_Start;
	bottomP0 = float3(bottom.x, bottom.y, bottom.z + hex.s);
	bottomP1 = float3(bottom.x + hex.h * 0.5f, bottom.y, bottom.z + hex.s * 0.5f);
	bottomP2 = float3(bottom.x + hex.h * 0.5f, bottom.y, bottom.z - hex.s * 0.5f);
	bottomP3 = float3(bottom.x, bottom.y, bottom.z - hex.s);
	bottomP4 = float3(bottom.x + hex.h * 0.5f, bottom.y, bottom.z - hex.s * 0.5f);
	bottomP5 = float3(bottom.x - hex.h * 0.5f, bottom.y, bottom.z + hex.s * 0.5f);

	//Triangle 1 topP0 -> bottomP0 -> bottomP1
	//Triangle 2 topP1 -> bottomP1 -> topP0
	//Triangle 3 topP1 -> bottomP1 -> bottomP2
	//Triangle 4 topP2 -> bottomP2 -> topP1
	//Triangle 5 topP2 -> bottomP2 -> bottomP3
	//Triangle 6 topP3 -> bootomP3 -> topP2
	//Triangle 7 topP3 -> bootomP3 -> bottomP4
	//Triangle 8 topP4 -> bootomP4 -> topP3
	//Triangle 9 topP4 -> bootomP4 -> bottomP5
	//Triangle 10 topP5 -> bootomP5 -> topP4
	//Triangle 11 topP5 -> bootomP5 -> bottomP0
	//Triangle 12 topP0 -> bootomP0 -> topP5

	
	tris[0].SetTriangle( &topP0, &bottomP0, &bottomP1, &tpln );
	tris[1].SetTriangle( &topP1, &bottomP1, &topP0, &tpln );
	tris[2].SetTriangle( &topP1, &bottomP1, &bottomP2, &rpln );
	tris[3].SetTriangle( &topP2, &bottomP2, &topP1, &rpln );
	tris[4].SetTriangle( &topP2, &bottomP2, &bottomP3, &tplnnz );
	tris[5].SetTriangle( &topP3, &bottomP3, &topP2, &tplnnz );
	tris[6].SetTriangle( &topP3, &bottomP3, &bottomP4, &tplnnznx );
	tris[7].SetTriangle( &topP4, &bottomP4, &topP3, &tplnnznx );
	tris[8].SetTriangle( &topP4, &bottomP4, &bottomP5, &rplnnx );
	tris[9].SetTriangle( &topP5, &bottomP5, &topP4, &rplnnx );
	tris[10].SetTriangle( &topP5, &bottomP5, &bottomP0, &tplnnx );
	tris[11].SetTriangle(&topP0, &topP0, &topP5, &tplnnx);
	tris[12].SetTriangle(&topP0, &topP1, &top, &topn);
	tris[13].SetTriangle(&topP1, &topP2, &top, &topn);
	tris[14].SetTriangle(&topP2, &topP3, &top, &topn);
	tris[15].SetTriangle(&topP3, &topP4, &top, &topn);
	tris[16].SetTriangle(&topP4, &topP5, &top, &topn);
	tris[17].SetTriangle(&topP5, &topP0, &top, &topn);
}

NewPlane planes[7];

void BuildPlanes(const Hexagon& hex, float r)
{
	if (planes[0].n.y != 0 || planes[1].n.x != 0.5f)
	{
		planes[0].n.x = 0;
		planes[0].n.y = 1;
		planes[0].n.z = 0;
		planes[1].n = tpln;
		planes[2].n = rpln;
		planes[3].n = tplnnz;
		planes[4].n = tplnnznx;
		planes[5].n = rplnnx;
		planes[6].n = tplnnx;
	}

	planes[0].p = hex.seg.m_End;
	planes[1].p = hex.seg.m_End;
	planes[2].p = hex.seg.m_End;
	planes[3].p = hex.seg.m_End;
	planes[4].p = hex.seg.m_End;
	planes[5].p = hex.seg.m_End;
	planes[6].p = hex.seg.m_End;
	
	planes[0].p.x += 0;
	planes[0].p.y += r;
	planes[0].p.z += 0;
				  
	planes[1].p.x += hex.h * 0.5f + tpln.x * r;
	planes[1].p.y += 0 + tpln.y * r;
	planes[1].p.z += hex.s * 0.5f + tpln.z * r;
				  
	planes[2].p.x += hex.h * 0.5f + rpln.x * r;
	planes[2].p.y += rpln.y * r;
	planes[2].p.z += hex.s * 0.5f + rpln.z * r;
				  
	planes[3].p.x += hex.h * 0.5f + tplnnz.x * r;
	planes[3].p.y += tplnnz.y * r;
	planes[3].p.z += -hex.s * 0.5f + tplnnz.z * r;
				  
	planes[4].p.x += -hex.h * 0.5f + tplnnznx.x * r;
	planes[4].p.y += tplnnznx.y * r;
	planes[4].p.z += -hex.s * 0.5f + tplnnznx.z * r;
				  
	planes[5].p.x += -hex.h * 0.5f + rplnnx.x * r;
	planes[5].p.y += rplnnx.y * r;
	planes[5].p.z += hex.s * 0.5f + rplnnx.z * r;
				  
	planes[6].p.x += -hex.h * 0.5f + tplnnx.x * r;
	planes[6].p.y += tplnnx.y * r;
	planes[6].p.z += hex.s * 0.5f + tplnnx.z * r;
}

AABB HexBox;
AABB SHexBox;
float3 HexagonToSphere(const Hexagon& hex, Sphere& s, float3& pastPos, float& Stime, ED2Mesh* mesh)
{
	HexBox.min.x = hex.seg.m_Start.x - 1;
	HexBox.min.y = hex.seg.m_Start.y;
	HexBox.min.z = hex.seg.m_Start.z - 1;
	HexBox.max.x = hex.seg.m_End.x + 1;
	HexBox.max.y = hex.seg.m_End.y + 3;
	HexBox.max.z = hex.seg.m_End.z + 1;
	SHexBox.min.x = s.m_Center.x - s.m_Radius - 1;
	SHexBox.min.y = s.m_Center.y - s.m_Radius - 1;
	SHexBox.min.z = s.m_Center.z - s.m_Radius - 1;
	SHexBox.max.x = s.m_Center.x + s.m_Radius + 1;
	SHexBox.max.y = s.m_Center.y + s.m_Radius + 1;
	SHexBox.max.z = s.m_Center.z + s.m_Radius + 1;
	if (!AABBtoAABB(SHexBox, HexBox))
		return zeroF;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	float3 top = hex.seg.m_End;
	float3 topP0 = float3(top.x, top.y, top.z + hex.s);
	float3 topP1 = float3(top.x + hex.h * 0.5f, top.y, top.z + hex.s * 0.5f);
	float3 topP2 = float3(top.x + hex.h * 0.5f, top.y, top.z - hex.s * 0.5f);
	float3 topP3 = float3(top.x, top.y, -hex.s);
	float3 topP4 = float3(top.x - hex.h * 0.5f, top.y, top.z - hex.s * 0.5f);
	float3 topP5 = float3(top.x - hex.h * 0.5f, top.y, top.z + hex.s * 0.5f);
	float3 bottom = hex.seg.m_Start;
	float3 bottomP0 = float3(bottom.x, bottom.y,bottom.z + hex.s);
	float3 bottomP1 = float3(bottom.x + hex.h * 0.5f, bottom.y, bottom.z + hex.s * 0.5f);
	float3 bottomP2 = float3(bottom.x + hex.h * 0.5f, bottom.y, bottom.z - hex.s * 0.5f);
	float3 bottomP3 = float3(bottom.x, bottom.y, bottom.z - hex.s);
	float3 bottomP4 = float3(bottom.x + hex.h * 0.5f, bottom.y, bottom.z - hex.s * 0.5f);
	float3 bottomP5 = float3(bottom.x - hex.h * 0.5f, bottom.y, bottom.z + hex.s * 0.5f);

	//Triangle 1 topP0 -> bottomP0 -> bottomP1
	//Triangle 2 topP1 -> bottomP1 -> topP0
	//Triangle 3 topP1 -> bottomP1 -> bottomP2
	//Triangle 4 topP2 -> bottomP2 -> topP1
	//Triangle 5 topP2 -> bottomP2 -> bottomP3
	//Triangle 6 topP3 -> bootomP3 -> topP2
	//Triangle 7 topP3 -> bootomP3 -> bottomP4
	//Triangle 8 topP4 -> bootomP4 -> topP3
	//Triangle 9 topP4 -> bootomP4 -> bottomP5
	//Triangle 10 topP5 -> bootomP5 -> topP4
	//Triangle 11 topP5 -> bootomP5 -> bottomP1
	//Triangle 12 topP1 -> bootomP1 -> topP5
	*/

	//useless triangle shit
	/*BuildTris(hex);

	float3 Sdirection = s.m_Center - pastPos;

	float time = 0;
	float3 outN = zeroF;
	bool test = false;
	float smallest = 1;
	float3 smallestN = zeroF;
	float dot = 1;

	for (int i = 0; i < 18; ++i)
	{
		test = false;
		dot = dot_product(Sdirection.normalize(), *tris[i].n);
		if (dot < 0)
		{
			test = IntersectMovingSphereTriangle(*tris[i].p0, *tris[i].p1, *tris[i].p2, *tris[i].n, pastPos, Sdirection, s.m_Radius, time, outN);
		}
		if (test)
		{
			if (time < smallest)
			{
				smallest = time;
				smallestN = *tris[i].n;
			}
		}
	}
	if(!smallestN.isEquil(zeroF) && smallest < Stime)
	{
		float3 NPos = pastPos + Sdirection * smallest;

		s.m_Center = NPos;
		Stime = smallest;
		return smallestN;
	}
	return zeroF;*/

	float3 Sdirection = s.m_Center - pastPos;
	
	
	/*
	if (mesh)
	{

		float3 CurRelitivePos = s.m_Center - hex.seg.m_Start;
		float3 PastRelitivePos = pastPos - hex.seg.m_Start;

		float time = 0;
		float3 outN = zeroF;

		bool test = IntersectMovingSphereMesh(PastRelitivePos, Sdirection, s.m_Radius, mesh, time, outN);

		if (test && time < 1 && time < Stime)
		{
			Stime = time;
			s.m_Center = pastPos + Sdirection * (time - 0.001f);

			return outN;
		}
	}
	*/
	//return zeroF;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float3 endPoint = s.m_Center;

	float3 startPoint = pastPos;

	BuildPlanes(hex, s.m_Radius);

	////////////////////////////////////////////////////////////////////////

	//If not moveing we need to add epsilon so that inf does not happen
	if (Sdirection.isEquil(float3(0, 0, 0)))
	{
		endPoint += 0.00001f;
		startPoint -= 0.00001f;
	}

	float times[7];
	float3 InPoints[7];

	bool allInside = true;
	for (int i = 0; i < 7; ++i)
	{
		int Stest = ClassifyPointToPlane(planes[i], startPoint);
		int Etest = ClassifyPointToPlane(planes[i], endPoint);
		if (Stest == 1 && Etest == 1)
		{
			if (i != 0)
			{
				return zeroF;
			}
			else
			{
				times[0] = -1;
				continue;
			}
		}
		if (Stest != -1 && i != 0)
		{
			allInside = false;
		}
		if (Stest >= 0 && Etest == -1)
		{
			InPoints[i] = LS2PI(startPoint, endPoint, planes[i], times[i]);
		}
		else
		{
			InPoints[i] = float3(100, 100, 100);
			times[i] = 100;
		}
	}

	if (allInside)
	{
		if (Sdirection.y <= 0)
		{
			if (planes[0].p.y > endPoint.y)
			{
				s.m_Center.y = hex.seg.m_End.y;
				Stime = 0.0001f;
				return planes[0].n;
			}
		}
	}

	int at = -1;

	int testInside;
	for (int i = 0; i < 7; ++i)
	{
		bool isInside = true;
		for (int j = 0; j < 7; ++j)
		{
			if (j == i || times[i] == 100)
			{
				continue;
			}
			testInside = ClassifyPointToPlane(planes[j], InPoints[i]);
			if (testInside > 0)
			{
				isInside = false;
				break;
			}
		}
		if (isInside)
		{
			if (at == -1 && times[i] < 1)
			{
				at = i;
			}
			else if (times[i] < times[at])
			{
				at = i;
			}
		}
	}

	if (at != -1 && times[at] < Stime)
	{
		if (dot_product(Sdirection, planes[at].n) < 0)
		{
			s.m_Center = pastPos + Sdirection * (times[at] - 0.001f);
			Stime = times[at];

			return planes[at].n;
		}
	}

	return zeroF;

	/*
	//If above top plane then no collision with anything return zero
	if (TstartTest == 1 && TendTest == 1)
	{
		return zeroF;
	}
	//If on top plane then just return top plane no need to move sphere
	if (TstartTest == 0 && TendTest == 0)
	{
		return toptest.n;
	}
	//check if within other planes
	if (TstartTest >= 0 && TendTest == -1)
	{
		float3 InPoint = LS2PI(startPoint, endPoint, toptest);
		int toprightresult = ClassifyPointToPlane(tpl, InPoint);
		int rightresult = ClassifyPointToPlane(rpl, InPoint);
		//If on both planes then collide with corner
		if (toprightresult == 0 && rightresult == 0)
		{
			if (relPos.x < 0)
			{
				InPoint.x *= -1;
				rpl.n.x *= -1;
				tpl.n.x *= -1;
			}
			if (relPos.z < 0)
			{
				InPoint.z *= -1;
				rpl.n.z *= -1;
				tpl.n.x *= -1;
			}

			float3 dff = InPoint - relPos;

			s.m_Center += dff;

			return (toptest.n + tpl.n + rpl.n).normalize();
		}
		//If in both then collides with top
		else if (toprightresult == -1 && rightresult == -1)
		{
			if (relPos.x < 0) InPoint.x *= -1;
			if (relPos.z < 0) InPoint.z *= -1;

			float3 dff = InPoint - relPos;

			s.m_Center += dff;

			return toptest.n;
		}
		//If on the edge of one of the planes
		else if (toprightresult == 0)
		{
			if (relPos.x < 0)
			{
				InPoint.x *= -1;
				tpl.n.x *= -1;
			}
			if (relPos.z < 0)
			{
				InPoint.z *= -1;
				tpl.n.x *= -1;
			}

			float3 dff = InPoint - relPos;

			s.m_Center += dff;

			return (toptest.n + tpl.n).normalize();
		}
		else if (rightresult == 0)
		{
			if (relPos.x < 0)
			{
				InPoint.x *= -1;
				rpl.n.x *= -1;
			}
			if (relPos.z < 0)
			{
				InPoint.z *= -1;
				rpl.n.z *= -1;
			}

			float3 dff = InPoint - relPos;

			s.m_Center += dff;

			return (toptest.n + rpl.n).normalize();
		}
	}
	//If below top or did not collide with top
	{
		//////////////////////Check if line passes through one of the side planes//////////////////////////////
		int TRstartTest = ClassifyPointToPlane(tpl, startPoint);
		int TRendTest = ClassifyPointToPlane(tpl, endPoint);
		//If above anyside plane then no collision with anyplane return zero
		if (TRstartTest == 1 && TRendTest == 1)
		{
			return zeroF;
		}
		if (TRstartTest == 0 && TRendTest == 0)
		{
			if (s.m_Center.x < hex.seg.m_Start.x)
			{
				tpl.n.x *= -1;
			}
			if (s.m_Center.z < hex.seg.m_Start.z)
			{
				tpl.n.x *= -1;
			}

			return tpl.n;
		}
		if (TRstartTest >= 0 && TRendTest == -1)
		{
			float3 InPoint = LS2PI(startPoint, endPoint, tpl);
			int TTest = ClassifyPointToPlane(toptest, InPoint);
			if (TTest == 1)
			{
				return zeroF;
			}
			int rightresult = ClassifyPointToPlane(rpl, InPoint);
			if (rightresult == 0)
			{
				if (relPos.x < 0)
				{
					InPoint.x *= -1;
					rpl.n.x *= -1;
					tpl.n.x *= -1;
				}
				if (relPos.z < 0)
				{
					InPoint.z *= -1;
					rpl.n.z *= -1;
					tpl.n.x *= -1;
				}

				float3 dff = InPoint - relPos;

				s.m_Center += dff;

				return (tpl.n + rpl.n).normalize();
			}
			else if (rightresult == -1)
			{
				if (relPos.x < 0)
				{
					InPoint.x *= -1;
					tpl.n.x *= -1;
				}
				if (relPos.z < 0)
				{
					InPoint.z *= -1;
					tpl.n.x *= -1;
				}

				float3 dff = InPoint - relPos;

				s.m_Center += dff;

				return tpl.n;
			}
		}
		//If below topright or did not collide with topright
		{
			int RstartTest = ClassifyPointToPlane(rpl, startPoint);
			int RendTest = ClassifyPointToPlane(rpl, endPoint);
			if (RstartTest == 1 && RendTest == 1)
			{
				//return zeroF;
			}
			if (RstartTest == 0 && RendTest == 0)
			{
				if (s.m_Center.x < hex.seg.m_Start.x)
				{
					rpl.n.x *= -1;
				}
				if (s.m_Center.z < hex.seg.m_Start.z)
				{
					rpl.n.x *= -1;
				}

				return rpl.n;
			}
			if (RstartTest >= 0 && RendTest == -1)
			{
				float3 InPoint = LS2PI(startPoint, endPoint, rpl);
				int TTest = ClassifyPointToPlane(toptest, InPoint);
				if (TTest == 1)
				{
					return zeroF;
				}
				if (relPos.x < 0)
				{
					InPoint.x *= -1;
					rpl.n.x *= -1;
				}
				if (relPos.z < 0)
				{
					InPoint.z *= -1;
					rpl.n.z *= -1;
				}

				float3 dff = InPoint - relPos;

				s.m_Center += dff;

				return rpl.n;
			}
		}
	}
	*/  //NewOld
	////////////////////////////////////////////////////////////////////Test Top & Bottom/////////////////////////////////////////////////////////////        OLD
	/*
	int topcify = ClassifySphereToPlane(toptest, s);
	if (topcify == 1) return zeroF;

	int bottomcify = ClassifySphereToPlane(bottomtest, s);

	if (bottomcify == 1) return zeroF;

	if (bottomcify == 2 && topcify == 2)
	{
		///////////////////////////////////////////////////////////////////////Test Sides/////////////////////////////////////////////////////////////////
		// Perform a sphere-to-plane test.
		// Returns 1 if the sphere is in front of the plane.
		// Returns 2 if the sphere is behind the plane.
		// Returns 3 if the sphere straddles the plane.
		Sphere Scopy;
		Scopy.m_Center = float3(abs(s.m_Center.x - SE.x), 0, abs(s.m_Center.z - SE.z));
		Scopy.m_Radius = s.m_Radius;
		int cify = ClassifySphereToPlane(tpl, Scopy);
		if (cify == 1)
		{
			return zeroF;
		}
		else if (cify == 2)
		{

			Sphere rScopy;
			rScopy.m_Center = float3(abs(s.m_Center.x - SE.x), 0, abs(s.m_Center.z - SE.z));
			rScopy.m_Radius = s.m_Radius;
			int rcify = ClassifySphereToPlane(rpl, rScopy);
			if (rcify == 1) return zeroF;
			else if (rcify == 2)
			{
				float3 topcheck = float3(rScopy.m_Center.x, hex.seg.m_End.y, rScopy.m_Center.z);
				NewPlane toppl;
				toppl.n = float3(0, 1, 0);
				toppl.p = topcheck;
				float3 p0 = SpherePlanePoint(toppl, rScopy);
				float3 p1 = SpherePlanePoint(rpl, rScopy);
				float3 p2 = SpherePlanePoint(tpl, rScopy);
				float pd0 = (p0 - rScopy.m_Center).magnitude();
				float pd1 = (p1 - rScopy.m_Center).magnitude();
				float pd2 = (p2 - rScopy.m_Center).magnitude();
				if (pd1 < pd2 && pd1 < pd0)
				{
					float3 dff = p1 - rScopy.m_Center;

					if (relPos.x < 0) dff.x *= -1;
					if (relPos.z < 0) dff.z *= -1;

					s.m_Center += dff - 0.00001f;

					if (s.m_Center.x < SE.x)
						rpl.n.x *= -1;
					if (s.m_Center.z < SE.z)
						rpl.n.z *= -1;

					return rpl.n;
				}
				else if(pd2 < pd0)
				{
					float3 dff = p2 - rScopy.m_Center;

					if (relPos.x < 0) dff.x *= -1;
					if (relPos.z < 0) dff.z *= -1;

					s.m_Center += dff - 0.00001f;

					if (s.m_Center.x < SE.x)
						tpl.n.x *= -1;
					if (s.m_Center.z < SE.z)
						tpl.n.z *= -1;

					return tpl.n;
				}
				else
				{
					float3 dff = p0 - rScopy.m_Center;

					if (relPos.x < 0) dff.x *= -1;
					if (relPos.z < 0) dff.z *= -1;

					s.m_Center += dff - 0.00001f;

					if (s.m_Center.x < SE.x)
						toppl.n.x *= -1;
					if (s.m_Center.z < SE.z)
						toppl.n.z *= -1;

					return toppl.n;
				}
			}
			else if (rcify == 3)
			{
				float3 cc = SpherePlanePoint(tpl, rScopy);

				float3 dff = cc - rScopy.m_Center;

				if (relPos.x < 0) dff.x *= -1;
				if (relPos.z < 0) dff.z *= -1;

				s.m_Center += dff - 0.00001f;

				if (s.m_Center.x < SE.x)
					rpl.n.x *= -1;
				if (s.m_Center.z < SE.z)
					rpl.n.z *= -1;

				return rpl.n;
			}
		}
		else if (cify == 3)
		{
			Sphere rScopy;
			rScopy.m_Center = float3(abs(s.m_Center.x - SE.x), 0, abs(s.m_Center.z - SE.z));
			rScopy.m_Radius = s.m_Radius;
			int rcify = ClassifySphereToPlane(rpl, rScopy);
			if (rcify == 1) return zeroF;
			if (rcify == 2)
			{
				float3 cc = SpherePlanePoint(tpl, rScopy);

				float3 dff = cc - rScopy.m_Center;

				if (relPos.x < 0) dff.x *= -1;
				if (relPos.z < 0) dff.z *= -1;

				s.m_Center += dff - 0.00001f;

				if (s.m_Center.x < SE.x)
					tpl.n.x *= -1;
				if (s.m_Center.z < SE.z)
					tpl.n.z *= -1;

				return tpl.n;
			}
			if (rcify == 3)
			{
				float3 p1 = SpherePlanePoint(rpl, rScopy);
				float3 p2 = SpherePlanePoint(tpl, rScopy);
				float pd1 = dot_product((p1 - rScopy.m_Center), (p1 - rScopy.m_Center));
				float pd2 = dot_product((p2 - rScopy.m_Center), (p2 - rScopy.m_Center));
				if (pd1 < pd2)
				{
					float3 dff = p1 - rScopy.m_Center;

					if (relPos.x < 0) dff.x *= -1;
					if (relPos.z < 0) dff.z *= -1;

					s.m_Center += dff - 0.00001f;

					if (s.m_Center.x < SE.x)
						rpl.n.x *= -1;
					if (s.m_Center.z < SE.z)
						rpl.n.z *= -1;

					return rpl.n;
				}
				else
				{
					float3 dff = p2 - rScopy.m_Center;

					if (relPos.x < 0) dff.x *= -1;
					if (relPos.z < 0) dff.z *= -1;

					s.m_Center += dff - 0.00001f;

					if (s.m_Center.x < SE.x)
						tpl.n.x *= -1;
					if (s.m_Center.z < SE.z)
						tpl.n.z *= -1;

					return tpl.n;
				}
			}
		}
	}

	if (topcify == 3)
	{
		Sphere rScopy;
		rScopy.m_Center = float3(abs(s.m_Center.x - SE.x), 0, abs(s.m_Center.z - SE.z));
		rScopy.m_Radius = s.m_Radius;
		Sphere Scopy;
		Scopy.m_Center = float3(abs(s.m_Center.x - SE.x), 0, abs(s.m_Center.z - SE.z));
		Scopy.m_Radius = s.m_Radius;
		int rcify = ClassifySphereToPlane(rpl, rScopy);
		int cify = ClassifySphereToPlane(tpl, Scopy);
		if (cify != 1 && rcify != 1)
		{
			s.m_Center = SpherePlanePoint(toptest, s);

			float3 n = float3(0, 1, 0);

			return n;
		}
	}
	*/
}
Sphere capShex;
float3 HexagonToCapsule(const Hexagon& hex, Capsule& c, float3& pastPos, float& time, ED2Mesh* mesh)
{
	capShex.m_Center = c.m_Segment.m_Start;
	capShex.m_Radius = c.m_Radius;
	float3 n = HexagonToSphere(hex, capShex, pastPos, time, mesh);
	if (!n.isEquil(zeroF))
	{
		float3 diff = c.m_Segment.m_Start - capShex.m_Center;
		diff = n * dot_product(diff, n);
		c.m_Segment.m_Start = c.m_Segment.m_Start - diff;
	}
	return n;
}

float3 XMtoF(DirectX::XMFLOAT3 m)
{
	float3 f;
	f.x = m.x;
	f.y = m.y;
	f.z = m.z;
	return f;
}

DirectX::XMFLOAT3 FtoXM(float3 f)
{
	DirectX::XMFLOAT3 m;
	m.x = f.x;
	m.y = f.y;
	m.z = f.z;
	return m;
}

bool QuadraticFormula(float a, float b, float c, float& u0, float& u1)
{
	float q = b * b - 4 * a * c;
	if (q >= 0)
	{
		float sq = sqrt(q);
		float d = 1 / (2 * a);
		u0 = (-b + sq) * d;
		u1 = (-b - sq) * d;
		return true;
	}
	return false;
}
