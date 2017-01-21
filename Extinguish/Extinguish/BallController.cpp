#include "BallController.h"

void BallController::OnTriggerEnter(Collider *obj)
{
	SphereCollider *col = dynamic_cast<SphereCollider*>(obj);

	if (col)
	{
		// set everything to them being the holder
		isHeld = true;
		holder = obj->GetGameObject();
	}
}

void BallController::Init()
{
	
}

void BallController::Update(float dt, InputManager* input)
{
	if (!isHeld)
		SetHolder(nullptr);

	int a = 0;

	a += 5;
}

void BallController::ThrowTo(GameObject *target)
{
	isHeld = false;
	holder = nullptr;

	float num = 15;
	float3 vel = float3(me->GetTransform()->GetForward().x * num, me->GetTransform()->GetForward().y * num, me->GetTransform()->GetForward().z * num);
	me->GetTransform()->AddVelocity(vel);
}

void BallController::DropBall(GameObject *person)
{
	isHeld = false;
	holder = nullptr;

	// add some velocity to me in the holders forward vec
	float num = 3;
	float3 vel = float3(person->GetTransform()->GetForward().x * num, person->GetTransform()->GetForward().y * num, person->GetTransform()->GetForward().z * num);
	me->GetTransform()->AddVelocity(vel);
}

bool  BallController::GetIsHeld()
{
	return isHeld;
}

GameObject* BallController::GetHolder()
{
	if (isHeld)
		return holder;

	return nullptr;
}

void BallController::SetIsHeld(bool ans)
{
	isHeld = ans;
}

void BallController::SetHolder(GameObject *person)
{
	if (isHeld)
	{
		holder = person;
		person->GetTransform()->AddChild(me->GetTransform());
	}
}