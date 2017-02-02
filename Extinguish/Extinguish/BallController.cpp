#include <iostream>
#include "BallController.h"
using namespace std;

#define ThrowSpeed 28
#define DropSpeed 10


void BallController::OnTriggerEnter(Collider *obj)
{
	SphereCollider *scol = dynamic_cast<SphereCollider*>(obj);

	// if i collide with a crosse
	//if (scol)
	//	SetHolder(obj->GetGameObject());
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

	if (!isHeld && transform->GetParent())
	{
		SetHolder(transform->GetParent()->GetGameObject());
	}

	if (isHeld && !isThrown)
		transform->SetVelocity(float3(0, 0, 0));

	else
		//transform->AddVelocity(float3(0, -9.8f * dt, 0));

	if (isThrown)
	{
		if (timer.TotalTime() > 0.28f)
		{
			isHeld = false;
			isThrown = false;
			holder = nullptr;
		}
	}

	//cout << isHeld;
}

void BallController::Throw()
{
	timer.Restart();
	isThrown = true;
	holder->GetTransform()->RemoveChild(me->GetTransform());
	holder = nullptr;
	transform->SetParent(nullptr);
}

void BallController::ThrowTo(GameObject *target)
{
	isHeld = false;
	holder->GetTransform()->RemoveChild(me->GetTransform());
	holder = nullptr;
	transform->SetParent(nullptr);

	float3 vel = me->GetTransform()->GetForwardf3() * ThrowSpeed;
	transform->AddVelocity(vel);
}

void BallController::DropBall(GameObject *person)
{
	isHeld = false;
	holder->GetTransform()->RemoveChild(me->GetTransform());
	holder = nullptr;
	transform->SetParent(nullptr);

	// add some velocity to me in the holders forward vec
	float3 vel = person->GetTransform()->GetForwardf3() * DropSpeed;
	transform->AddVelocity(vel);
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

	transform->SetPosition(float3(0, 0, 0.1f));
	person->GetTransform()->AddChild(me->GetTransform());
	transform->SetParent(person->GetTransform());
}