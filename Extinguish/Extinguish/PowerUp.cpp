#include "PowerUp.h"
#include "Collider.h"
#include "CapsuleCollider.h"
#include "GameObject.h"
#include "PlayerController.h"
#include "Renderer.h"
#include "PowerUpEvent.h"
#include "EventDispatcher.h"

//structors//
PowerUp::PowerUp()
{
	timer = 0.0f;
	isActivated = false;
}

//basic//
void PowerUp::Update(float _dt)
{
	if (isActivated)
	{
		timer += _dt;
	}

	if (IsDone() && isActivated)
	{
		Deactivate();
	}
}

void PowerUp::Shutdown()
{
	
}

void PowerUp::HandleEvent(Event* e)
{

}

void PowerUp::OnTriggerEnter(Collider* collider)
{
	if (!isActivated)
	{
		if (collider->GetColliderType() == Collider::ColliderType::CTCapsule)
		{
			CapsuleCollider* capsCollider = (CapsuleCollider*)collider;

			if (capsCollider->GetGameObject()->GetName().find("Mage") != string::npos)
			{
				player = capsCollider->GetGameObject()->GetComponent<PlayerController>();

				isActivated = true;
				Activate();

				//stop rendering
				GetGameObject()->GetComponent<Renderer>()->SetEnabled(false);

				//tell powerup manager it was picked up
				PowerUpEvent* powerEvent = new PowerUpEvent(GetGameObject()->GetName(), true, managerIndex);
				EventDispatcher::GetSingleton()->DispatchTo(powerEvent, "PowerUpManager");
				delete powerEvent;

				return;
			}
		}
	}
}

//misc//
void PowerUp::Activate()
{
	//overwrite this
}

void PowerUp::Deactivate()
{
	//maybe call this in derived
	//and in here, destroy the component and/or object completely so it stops updating and what not

	isActivated = false;
}

//getters//
float PowerUp::GetDuration()
{
	return duration;
}

bool PowerUp::IsDone()
{
	bool result = false;

	if (timer >= duration)
	{
		result = true;
	}

	return result;
}

PlayerController* PowerUp::GetPlayer()
{
	return player;
}

int PowerUp::GetManagerIndex()
{
	return managerIndex;
}

//setters//
void PowerUp::SetDuration(float dur)
{
	duration = dur;
}

void PowerUp::SetManagerIndex(int index)
{
	managerIndex = index;
}
