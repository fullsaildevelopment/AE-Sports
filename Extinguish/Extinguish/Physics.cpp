#include "Physics.h"
#include "GameObject.h"
#include "Transform.h"

void Physics::Init()
{
	//cache
	transform = GetGameObject()->GetTransform();

	isKinematic = false;
}

void Physics::FixedUpdate(float dt)
{
	if (!isKinematic)
	{
		float3 nV = transform->GetVelocity();
		//apply gravity
		if (!colliding)
		{
			nV += { 0, gravity * dt, 0 };
			nV += transform->GetVelocity() * (-airdrag * dt);
		}
		//apply friction if touching floor
		else
		{
			nV += transform->GetVelocity() * -1 * friction * dt;
			if (nV.y <= 3.0f && nV.y >= -3.0f)
			{
				nV.y = 0;
				resting = true;
			}
		}
		if (nV.x <= 0.057f && nV.x >= -0.057f)
		{
			nV.x = 0;
		}
		if (nV.y <= 0.06f && nV.y >= -0.06f)
		{
			nV.y = 0;
		}
		if (nV.z <= 0.057f && nV.z >= -0.057f)
		{
			nV.z = 0;
		}
		if ((nV * float3(1,0,1)).magnitude() > maxMoveSpeed)
		{
			float3 tm = (nV * float3(1, 0, 1)).normalize() * maxMoveSpeed;
			nV.x = tm.x;
			nV.z = tm.z;
		}
		transform->SetVelocity(nV);
		colliding = false;
		transform->Translate({ nV.x * dt, nV.y * dt, nV.z * dt });
	}
}

void Physics::HandlePhysics(Transform* tt, float3 nV, float3 nP, bool b = false)
{
	if (b)
	{
		nV *= bounce;
	}
	tt->SetVelocity(nV);
	tt->SetPosition(nP);
	colliding = true;
	if (!nV.isEquil(float3(0, 0, 0)))
		resting = false;
}

//getters//
bool Physics::IsKinematic()
{
	return isKinematic;
}

//setters//
void Physics::SetIsKinematic(bool toggle)
{
	isKinematic = toggle;
}