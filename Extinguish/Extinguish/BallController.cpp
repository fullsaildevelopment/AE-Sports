#include <iostream>
#include "BallController.h"
using namespace std;

void BallController::OnTriggerEnter(Collider *obj)
{
	SphereCollider *scol = dynamic_cast<SphereCollider*>(obj);

	// if i collide with a crosse
	if (scol)
	{
		// set everything to them being the holder
		isHeld = true;
		holder = obj->GetGameObject();
	}

	CapsuleCollider *col = dynamic_cast<CapsuleCollider*>(obj);

	// if i bump into a player, they caught me
	if (col)
	{
		if (col->GetGameObject()->GetTag() == "Team1" || col->GetGameObject()->GetTag() == "Team2" && !isHeld)
		{
			isHeld = true;
			holder = obj->GetGameObject();
		}
	}
}

BallController::BallController(GameObject* obj) : Component(obj)
{
	me = obj;
}

void BallController::Init()
{

}

void BallController::Update(float dt)
{
	timer.Signal();

	if (isHeld && !isThrown)
	{
		me->GetTransform()->SetVelocity(float3(0, 0, 0));
	}
	else
		me->GetTransform()->AddVelocity(float3(0, -9.8f * dt, 0));

	if (isThrown)
	{
		if (timer.TotalTime() > 0.09f)
		{
			isHeld = false;
			isThrown = false;
			holder = nullptr;
		}
	}

	//printf("%f %f %f \n", me->GetTransform()->GetVelocity().x, me->GetTransform()->GetVelocity().y, me->GetTransform()->GetVelocity().z);

}

void BallController::Throw()
{
	timer.Restart();
	isThrown = true;
	holder->GetTransform()->RemoveChild(me->GetTransform());
	holder = nullptr;
	me->GetTransform()->SetParent(nullptr);
}

void BallController::ThrowTo(GameObject *target)
{
	isHeld = false;
	holder->GetTransform()->RemoveChild(me->GetTransform());
	holder = nullptr;
	me->GetTransform()->SetParent(nullptr);

	float num = 15;
	float3 vel = me->GetTransform()->GetForwardf3() * num; // ***************************************************************************************
	me->GetTransform()->AddVelocity(vel);
}

void BallController::DropBall(GameObject *person)
{
	isHeld = false;
	holder = nullptr;

	// add some velocity to me in the holders forward vec
	float num = 3;
	float3 vel = person->GetTransform()->GetForwardf3() * num;
	me->GetTransform()->AddVelocity(vel);
}

bool  BallController::GetIsHeld()
{
	return isHeld;
}

GameObject* BallController::GetHolder()
{
	return holder;
}

void BallController::SetIsHeld(bool ans)
{
	isHeld = ans;
}

void BallController::SetHolder(GameObject *person)
{
	isHeld = true;
	holder = person;
	me->GetTransform()->SetPosition(float3(0, 0, 0));
	person->GetTransform()->AddChild(me->GetTransform());
	me->GetTransform()->SetParent(person->GetTransform());
}