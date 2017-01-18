#include "BallController.h"

//void Collide(Collider *obj, Collider *me)
//{
//	// if i collide with a persons crosse
//	SphereCollider *col = dynamic_cast<SphereCollider*>(obj);
//
//	if (col)
//	{
//		//BallController::SetIsHeld(true);
//		
//		
//	}
//		// set everything to them being the holder etc.
//}

void BallController::Init()
{
	//me->OnCollisionEnter = Collide;
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

	float num = 5;
	XMFLOAT3 vel = XMFLOAT3(me->GetTransform()->GetForward().x * num, me->GetTransform()->GetForward().y * num, me->GetTransform()->GetForward().z * num);
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