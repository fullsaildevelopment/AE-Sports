#include <iostream>
#include "BallController.h"
using namespace std;

void BallController::OnTriggerEnter(Collider *obj)
{
	SphereCollider *col = dynamic_cast<SphereCollider*>(obj);

	// if i collide with a crosse
	if (col)
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

void BallController::Update(float dt, InputManager* input)
{
	if (isHeld)
	{
		me->GetTransform()->SetVelocity(float3(0, 0, 0));
	}
}

void BallController::ThrowTo(GameObject *target)
{
	isHeld = false;
	holder = nullptr;

	float num = 15;
	float3 vel = me->GetTransform()->GetForwardf3() * num;
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
	person->GetTransform()->AddChild(me->GetTransform());
}