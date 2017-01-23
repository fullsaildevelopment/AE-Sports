#include <iostream>
#include "BallController.h"
using namespace std;

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

BallController::BallController(GameObject* o) : Component(o)
{
	me = o;
}

void BallController::Init()
{

}

void BallController::Update(float dt, InputManager* input)
{
	//if (isHeld)
	//{
	//	me->GetTransform()->SetVelocity(float3(0, 0, 0));
	//}
	//else me->GetTransform()->AddVelocity(float3(0, -9.8f * dt, 0));

	//cout << me->GetTransform()->GetPosition().x << " " << me->GetTransform()->GetPosition().y << " " << me->GetTransform()->GetPosition().z << endl;

	int a = 0;

	a += 5;
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