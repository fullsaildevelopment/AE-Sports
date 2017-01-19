#pragma once

#include <math.h>
#include <algorithm>
#include "vec3.h"
#include "matrix4.h"
#include <DirectXMath.h>

struct Plane
{
	vec3f normal;
	float offset;
};

struct AABB
{
	vec3f min;
	vec3f max;
};

struct Frustum
{	
	Plane planes[6];
	vec3f corners[8];
};

struct Segment
{
	vec3f m_Start;
	vec3f m_End;
};

struct Sphere
{
	vec3f m_Center;
	float m_Radius;
};

struct Capsule
{
	Segment m_Segment;
	float m_Radius;
};

enum FrustumCorners{ FTL = 0, FBL, FBR, FTR, NTL, NTR, NBR, NBL };
enum FrustumPlanes{ NEAR_PLANE = 0, FAR_PLANE, LEFT_PLANE, RIGHT_PLANE, TOP_PLANE, BOTTOM_PLANE };

void ComputePlane(Plane &plane, const vec3f& pointA, const vec3f& pointB, const vec3f &pointC);

int ClassifyPointToPlane(const Plane& plane, const vec3f& point);

int ClassifySphereToPlane(const Plane& plane, const Sphere& sphere);

int ClassifyAabbToPlane(const Plane& plane, const AABB& aabb);

int ClassifyCapsuleToPlane(const Plane& plane, const Capsule& capsule);

void BuildFrustum( Frustum& frustum, float fov, float nearDist, float farDist, float ratio, const matrix4f& camXform );

bool FrustumToSphere(const Frustum& frustum, const Sphere& sphere);

bool FrustumToAABB(const Frustum& frustum, const AABB& aabb);

bool FrustumToCapsule(const Frustum& frustum, const Capsule& capsule);

bool AABBtoAABB(const AABB& lhs, const AABB& rhs);

bool SphereToSphere(const Sphere& lhs, const Sphere& rhs);

bool SphereToAABB(const Sphere& lhs, const AABB& rhs);

bool CapsuleToSphere(const Capsule& capsule, const Sphere& sphere);

bool AABBToCapsule(const AABB& box, const Capsule& cap);

bool CapsuleToCapsule(const Capsule& capl, const Capsule& capr);

float SweptAABBtoAABB(const AABB& boxl, const AABB& boxr, float3 vel, float& normalx, float& normaly, float& normalz);

float3 SweptSpheretoAABB(Sphere& s, const AABB& b, float3& vel);

float3 SweptSpheretoSphere(Sphere& s, const Sphere& ss, float3& vel);

bool SweptSpheretoSweptSphere(Sphere& sl, Sphere& sr, float3& vell, float3& velr);

bool SweptCaptoSweptCap(Capsule& cl, Capsule& cr, float3& vell, float3& velr, float3& pos, float3& opos);

bool AABBToCapsuleReact(const AABB& box, Capsule& cap, float3& vel, float3& pos);

float3 XMtoF(DirectX::XMFLOAT3 m);

DirectX::XMFLOAT3 FtoXM(float3 f);

bool QuadraticFormula(float a, float b, float c, float& u0, float& u1);