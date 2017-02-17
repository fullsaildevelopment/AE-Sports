#include <iostream>
#include "BallController.h"
#include "Physics.h"
#include "GameObject.h"
#include "InputManager.h"

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
	//printf("%f %f %f \n", transform->GetVelocity().x, transform->GetVelocity().y, transform->GetVelocity().z);
	timer.Signal();

	if (!isHeld && transform->GetParent())
	{
		SetHolder(transform->GetParent()->GetGameObject());
	}


#if _DEBUG
	if (InputManager::GetSingleton()->GetKey(17))
	{
		SetHolder(GetGameObject()->FindGameObject("Crosse1"));
	}
#endif

	if (isHeld && !isThrown && !transform->GetPosition().isEquil(float3( 0,0,0 )))
		transform->SetPosition({ 0,0,0 });

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

	// add some velocity to me in the holders forward vec
	//TODO: temp taken out to test... maybe might not need to re add with new physics though
	float3 vel = holder->GetTransform()->GetForwardf3() * 1;
	vel.y += 1.0f;
	transform->AddVelocity(vel);

	//need to do this to prevent recolliding with crosse net
	transform->SetPosition({ transform->GetWorld()._41, transform->GetWorld()._42, transform->GetWorld()._43 });
	transform->SetRotation({ person->GetTransform()->GetRotation().x, person->GetTransform()->GetRotation().y, person->GetTransform()->GetRotation().z });

	const float metersForward = 1.0f;
	XMFLOAT3 translation = holder->GetTransform()->GetForward();
	transform->Translate({ translation.x * metersForward, translation.y * metersForward, translation.z * metersForward });

	//set ball variables
	isHeld = false;
	holder->GetTransform()->RemoveChild(me->GetTransform());
	holder = nullptr;
	transform->SetParent(nullptr);

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
	if (person)
	{
		isHeld = true;
		holder = person;

		//turn off physics
		physics->SetIsKinematic(true);
		GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(false);

		transform->SetParent(person->GetTransform()); //set parent adds a child
		transform->GetWorld();
		transform->SetVelocity(float3(0, 0, 0));
		transform->SetPosition(float3(0, 0, 0));
	}
	else
	{
		isHeld = false;
		holder = person;
		if (transform->GetParent())
			transform->GetParent()->RemoveChild(transform);
		transform->SetParent(nullptr);
		physics->SetIsKinematic(false);
		GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(true);
	}
}