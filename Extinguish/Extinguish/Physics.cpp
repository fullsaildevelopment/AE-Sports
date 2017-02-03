#include "Physics.h"
#include "GameObject.h"

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
		transform->AddVelocity({ 0.0f, gravity * dt, 0.0f });

		//apply friction if touching floor
		//transform->AddVelocity({
	}
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