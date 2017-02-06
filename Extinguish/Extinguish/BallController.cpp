#include <iostream>
#include "BallController.h"
#include "Physics.h"

using namespace std;

#define ThrowSpeed 28
#define DropSpeed 10


void BallController::OnTriggerEnter(Collider *obj)
{
	//SphereCollider *scol = dynamic_cast<SphereCollider*>(obj);

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
	//cache
	transform = GetGameObject()->GetTransform();
	physics = GetGameObject()->GetComponent<Physics>();
}

void BallController::Update(float dt)
{
	timer.Signal();

	if (!isHeld && transform->GetParent())
	{
		SetHolder(transform->GetParent()->GetGameObject());
	}

	//if (isHeld && !isThrown)
	//	transform->SetVelocity(float3(0, 0, 0));

	//else
		//transform->AddVelocity(float3(0, -9.8f * dt, 0));

	if (isThrown)
	{
		if (timer.TotalTime() > 0.31f)
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

	//turn on physics
	physics->SetIsKinematic(false);
	GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(true);
}

void BallController::ThrowTo(GameObject *target)
{
	isHeld = false;
	holder->GetTransform()->RemoveChild(me->GetTransform());
	holder = nullptr;
	transform->SetParent(nullptr);

	float3 vel = me->GetTransform()->GetForwardf3() * ThrowSpeed;
	transform->AddVelocity(vel);

	//turn on physics
	physics->SetIsKinematic(false);
	GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(true);
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

	//turn on physics
	physics->SetIsKinematic(false);
	GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(true);
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

	person->GetTransform()->AddChild(me->GetTransform());
	transform->SetParent(person->GetTransform());
	transform->SetVelocity(float3(0, 0, 0));
	transform->SetPosition(float3(0, 0, 0));

	//turn off physics
	physics->SetIsKinematic(true);
	GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(false);
}