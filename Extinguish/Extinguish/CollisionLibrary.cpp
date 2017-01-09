#include "CollisionLibrary.h"
#include <math.h>

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

// ComputePlane
//
// Calculate the plane normal and plane offset from the input points
void ComputePlane(Plane &plane, const vec3f& pointA, const vec3f& pointB, const vec3f &pointC)
{
	cross_product(plane.normal, pointB - pointA, pointC - pointB);
	plane.normal.normalize();
	plane.offset = dot_product(pointA,plane.normal);
}

// ClassifyPointToPlane
//
// Perform a half-space test. Returns 1 if the point is on or in front of the plane.
// Returns 2 if the point is behind the plane.
int ClassifyPointToPlane(const Plane& plane, const vec3f& point)
{
	return (dot_product(point, plane.normal) >= plane.offset) ? 1 : 2;
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
	sphere.m_Radius = dot_product(aabb.max - sphere.m_Center, vec3f(abs(plane.normal.x),abs(plane.normal.y),abs(plane.normal.z)));
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
void BuildFrustum( Frustum& frustum, float fov, float nearDist, float farDist, float ratio, const matrix4f& camXform )
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
	if(ClassifySphereToPlane(frustum.planes[NEAR_PLANE], sphere)   == 2) return false;
	if(ClassifySphereToPlane(frustum.planes[FAR_PLANE], sphere)	   == 2) return false;
	if(ClassifySphereToPlane(frustum.planes[LEFT_PLANE], sphere)   == 2) return false;
	if(ClassifySphereToPlane(frustum.planes[RIGHT_PLANE], sphere)  == 2) return false;
	if(ClassifySphereToPlane(frustum.planes[TOP_PLANE], sphere)	   == 2) return false;
	if(ClassifySphereToPlane(frustum.planes[BOTTOM_PLANE], sphere) == 2) return false;
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
	if(dot_product(lhs.m_Center - rhs.m_Center, lhs.m_Center - rhs.m_Center) > powf(lhs.m_Radius + rhs.m_Radius,2))
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