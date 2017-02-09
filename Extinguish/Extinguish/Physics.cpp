#include "Physics.h"
#include "GameObject.h"
#include "Transform.h"

void Physics::Init()
{
	//cache
	transform = GetGameObject()->GetTransform();

	isKinematic = false;
}

void Physics::Update(float dt)
{
	if (!isKinematic)
	{
		//apply gravity
		if (!colliding)
		{
			transform->AddVelocity({ 0, gravity * dt, 0 });
			transform->AddVelocity(transform->GetVelocity() * (-airdrag * dt));
		}
		//apply friction if touching floor
		else
		{
			transform->AddVelocity(transform->GetVelocity() * -1 * friction * dt);
		}
	}
	colliding = false;
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