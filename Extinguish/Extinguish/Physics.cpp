#include "Physics.h"
#include "GameObject.h"
#include "Transform.h"

void Physics::Init()
{
	//cache
	transform = GetGameObject()->GetTransform();

	isKinematic = false;
	hasMaxSpeed = true;
}

void Physics::FixedUpdate(float _dt)
{
	if (!isKinematic)
	{
		float3 nV = transform->GetVelocity();
		pastPos = transform->GetPosition();
		//apply gravity
		if (!colliding || stillapplygravity)
		{
			nV += { 0, gravity * _dt, 0 };
			nV += transform->GetVelocity() * (-airdrag * _dt);
		}
		//apply friction if touching
		if(colliding && !stillapplygravity)
		{
			nV += transform->GetVelocity() * -1 * friction * _dt;
		}
		if (nV.x <= 0.0001f && nV.x >= -0.0001f)
		{
			nV.x = 0;
		}
		if (nV.y <= 0.0001f && nV.y >= -0.0001f)
		{
			nV.y = 0;
		}
		if (nV.z <= 0.0001f && nV.z >= -0.0001f)
		{
			nV.z = 0;
		}

		//prevents x and z of object going above maxSpeed (y isn't affected)
		if ((nV * float3(1,0,1)).magnitude() > maxMoveSpeed && hasMaxSpeed)
		{
			float3 tm = (nV * float3(1, 0, 1)).normalize() * maxMoveSpeed;
			nV.x = tm.x;
			nV.z = tm.z;
		}
		transform->SetVelocity(nV);
		colliding = false;
		transform->Translate({ nV.x * _dt, nV.y * _dt, nV.z * _dt });
	}
}

void Physics::HandlePhysics(Transform* tt, float3 nV, float3 nP, bool _bounce, float3 collisionNormal, bool _stillApplyGravity, bool stopMovement)
{
	if (_bounce)
	{
		nV = nV - collisionNormal * bounce * dot_product(nV, collisionNormal * bounce);
	}
	if (stopMovement)
	{
		float3 invN = collisionNormal.negate();
		invN = invN * (nV * collisionNormal).magnitude();
		nV = nV - invN;
	}
	tt->SetVelocity(nV);
	pastPos = nP;
	tt->SetPosition(nP);
	stillapplygravity = _stillApplyGravity;
	colliding = true;
}

//getters//
bool Physics::IsKinematic()
{
	return isKinematic;
}

bool Physics::HasMaxSpeed()
{
	return hasMaxSpeed;
}

float Physics::GetMaxSpeed()
{
	return maxMoveSpeed;
}

//setters//
void Physics::SetIsKinematic(bool toggle)
{
	isKinematic = toggle;
}

void Physics::SetHasMaxSpeed(bool toggle)
{
	hasMaxSpeed = toggle;
}

void Physics::SetMaxSpeed(float newMaxSpeed)
{
	maxMoveSpeed = newMaxSpeed;
}