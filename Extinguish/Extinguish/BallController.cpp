#include <iostream>
#include "BallController.h"
using namespace std;

#define ThrowSpeed 28
#define DropSpeed 10


void BallController::OnTriggerEnter(Collider *obj)
{
	SphereCollider *scol = dynamic_cast<SphereCollider*>(obj);

	// if i collide with a crosse
	if (scol)
		SetHolder(obj->GetGameObject());
}

BallController::BallController(GameObject* obj) : Component(obj)
{
	me = obj;
}

void BallController::Init()
{
	transform = GetGameObject()->GetTransform();
}

void BallController::Update(float dt)
{
	timer.Signal();

	//cout << isHeld;

	if (!isHeld && transform->GetParent())
	{
		SetHolder(transform->GetParent()->GetGameObject());
	}

	if (isHeld && !isThrown)
		me->GetTransform()->SetVelocity(float3(0, 0, 0));

	else
		me->GetTransform()->AddVelocity(float3(0, -9.8f * dt, 0));

	if (isThrown)
	{
		if (timer.TotalTime() > 0.28f)
		{
			isHeld = false;
			isThrown = false;
			holder = nullptr;
		}
	}
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

	float3 vel = me->GetTransform()->GetForwardf3() * ThrowSpeed;
	me->GetTransform()->AddVelocity(vel);
}

void BallController::DropBall(GameObject *person)
{
	isHeld = false;
	holder->GetTransform()->RemoveChild(me->GetTransform());
	holder = nullptr;
	me->GetTransform()->SetParent(nullptr);

	// add some velocity to me in the holders forward vec
	float3 vel = person->GetTransform()->GetForwardf3() * DropSpeed;
	me->GetTransform()->AddVelocity(vel);
}

bool  BallController::GetIsHeld()
{
	return isHeld;
}

bool BallController::GetIsThrown()
{
	return isThrown;
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