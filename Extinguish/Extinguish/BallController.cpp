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

void BallController::Update()
{
	if (!isHeld)
		SetHolder(nullptr);
}

void BallController::ThrowTo(GameObject *target)
{
	isHeld = false;
	holder = nullptr;

	float num = 15;
	XMFLOAT3 vel = XMFLOAT3(me->GetTransform()->GetForward().x * num, me->GetTransform()->GetForward().y * num, me->GetTransform()->GetForward().z * num);
	me->GetTransform()->AddVelocity(vel);
}

void BallController::DropBall(GameObject *person)
{
	isHeld = false;
	holder = nullptr;

	// add some velocity to me in the holders forward vec
	float num = 3;
	XMFLOAT3 vel = XMFLOAT3(person->GetTransform()->GetForward().x * num, person->GetTransform()->GetForward().y * num, person->GetTransform()->GetForward().z * num);
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
}

void  BallController::SetIsHeld(bool ans)
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