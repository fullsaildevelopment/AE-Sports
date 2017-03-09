#include <iostream>
#include "BallController.h"
#include "Physics.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Crosse.h"
#include "SoundEngine.h"
#include "SoundEvent.h"
#include "EventDispatcher.h"

using namespace std;

#define ThrowSpeed 28
#define DropSpeed 10

void BallController::OnTriggerEnter(Collider *obj)
{
	//SphereCollider *scol = dynamic_cast<SphereCollider*>(obj);

	// if i collide with a crosseds
	//if (scol)
	//	SetHolder(obj->GetGameObject());
}

void BallController::OnCollisionEnter(Collider* obj)
{
	//play sound when it hits floor, so I can test 3D sound
	if (obj->GetGameObject()->GetName() == "MeterBox6" || obj->GetGameObject()->GetName() == "HexFloor")
	{
		SoundEvent* soundEvent = new SoundEvent();
		soundEvent->Init(AK::EVENTS::PLAY_BOING, GetGameObject()->FindIndexOfGameObject(GetGameObject()));
		EventDispatcher::GetSingleton()->DispatchTo(soundEvent, "Game");
		delete soundEvent;
	}
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

void BallController::LateInit()
{
	std::vector<GameObject*> go = *GetGameObject()->GetGameObjects();
	int size = (int)go.size();
	for (int i = 0; i < size; ++i)
	{
		Crosse* c = go[i]->GetComponent<Crosse>();
		if (c)
		{
			nets.push_back(c->GetGameObject()->GetTransform());
		}
	}
}

void BallController::Update(float dt)
{
	//printf("%f %f %f \n", transform->GetVelocity().x, transform->GetVelocity().y, transform->GetVelocity().z);
	timer.Signal();

	if (!isHeld && transform->GetParent())
	{
		SetHolder(transform->GetParent()->GetGameObject());
		holder->GetComponent<Crosse>()->SetColor(true);
	}

	if (InputManager::GetSingleton()->GetKey(17))
	{
		//SetHolder(GetGameObject()->FindGameObject("Crosse1"));
		GetGameObject()->FindGameObject("Crosse1")->GetComponent<Crosse>()->SetColor(true);
		GetGameObject()->FindGameObject("Crosse1")->GetComponent<Crosse>()->Catch();
	}

	if (isHeld && !isThrown && !transform->GetPosition().isEquil(float3( 0,0,0 )))
		transform->SetPosition({ 0,0,0 });

	if (isThrown)
	{
		if (timer.TotalTime() > 0.42f)
		{
			isHeld = false;
			isThrown = false;
			holder = nullptr;
		}
	}

	if (thrower)
	{
		timeSinceThrown += dt;
	}

	if (prevThrower)
	{
		timeSincePreviouslyThrown += dt;
	}

	//cout << thrower << " " << prevThrower << endl;
	//cout << timeSinceThrown << " " << timeSincePreviouslyThrown << endl;
	//cout << isHeld;
}

void BallController::FixedUpdate(float dt)
{
	if (transform->GetWorld()._42 < -0.3f)
	{
		transform->SetPosition(float3(transform->GetPosition().x, 1.5f, transform->GetPosition().z));
	}
	if (!isHeld && !isThrown)
	{
		int s = (int)nets.size();
		for (int i = 0; i < s; ++i)
		{
			XMFLOAT4X4 ball = *me->GetTransform()->GetWorldP();
			XMFLOAT4X4 net = *nets[i]->GetWorldP();
			float3 ball2net = float3(net._41, net._42, net._43) - float3(ball._41, ball._42, ball._43);
			float l = ball2net.magnitude();
			if (l < 3)
			{
				float s = 1 / l;
				s > 1.5f ? s = 1.5f : s < 0.0f ? s = 0.0f : s = s;
				me->GetTransform()->AddVelocity(ball2net.normalize() * s);
			}
		}
	}
}

void BallController::Throw()
{
	//store prevThrower even if null
	prevThrower = thrower;

	//store thrower before it's thrown
	thrower = holder;

	//set times
	timeSincePreviouslyThrown = timeSinceThrown;
	timeSinceThrown = 0;

	timer.Restart();
	isThrown = true;
	holder->GetTransform()->RemoveChild(me->GetTransform());
	holder->GetComponent<Crosse>()->SetColor(false);
	holder = nullptr;
	transform->SetParent(nullptr);

	//turn on physics
	physics->SetIsKinematic(false);
	GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(true);
}

void BallController::ThrowTo(GameObject *target)
{
	//store prevThrower even if thrower is null
	prevThrower = thrower;

	//store thrower before it's thrown
	thrower = holder;

	//set times
	timeSincePreviouslyThrown = timeSinceThrown;
	timeSinceThrown = 0;

	isHeld = false;
	holder->GetTransform()->RemoveChild(me->GetTransform());
	holder->GetComponent<Crosse>()->SetColor(false);
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
	holder->GetComponent<Crosse>()->SetColor(false);
	holder = nullptr;
	transform->SetParent(nullptr);
	thrower = nullptr;
	timeSinceThrown = 0;

	//turn on physics
	physics->SetIsKinematic(false);
	GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(true);
	timer.Restart();
	isThrown = true;
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
	if (!isThrown && holder)
		return holder->GetTransform()->GetParent()->GetParent()->GetGameObject();
}

GameObject* BallController::GetCrosseHolder()
{
	return holder;
}

GameObject* BallController::GetThrower()
{
	return thrower;
}

GameObject* BallController::GetPreviousThrower()
{
	return prevThrower;
}

float BallController::GetTimeSinceThrown()
{
	return timeSinceThrown;
}

float BallController::GetTimeSincePreviouslyThrown()
{
	return timeSincePreviouslyThrown;
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
		//prevThrower = thrower;
		//thrower = holder;

		//turn off physics
		physics->SetIsKinematic(true);
		GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(false);

		transform->SetParent(person->GetTransform()); //set parent adds a child
		transform->GetWorld();
		transform->SetVelocity(float3(0, 0, 0));
		transform->SetPosition(float3(0, 0, 0));
	}
	else //this isn't the same as throwing or dropping. Just no one has it anymore
	{
		prevThrower = nullptr;
		thrower = nullptr;
		timeSinceThrown = 0;
		timeSincePreviouslyThrown = 0;

		isHeld = false;
		holder = nullptr;
		if (transform->GetParent())
			transform->GetParent()->RemoveChild(transform);
		transform->SetParent(nullptr);
		physics->SetIsKinematic(false);
		GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(true);
	}
}