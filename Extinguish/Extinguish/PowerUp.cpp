#include "PowerUp.h"
#include "Collider.h"
#include "CapsuleCollider.h"
#include "GameObject.h"
#include "PlayerController.h"
#include "Renderer.h"
#include "PowerUpEvent.h"
#include "EventDispatcher.h"
#include "SoundEvent.h"
#include "SoundEngine.h"
#include "SphereCollider.h"
#include "AI.h"
//#include "BoxCollider.h"

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
		if (ResourceManager::GetSingleton()->IsServer())
			Deactivate();
		else
			isActivated = false;
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
				AI* ai = capsCollider->GetGameObject()->GetComponent<AI>();

				//don't allow AI to get powerup
				if (player && !ai)
				{
					isActivated = true;
					timer = 0.0f;

					Activate();

					//stop rendering
					GetGameObject()->GetComponent<Renderer>()->SetEnabled(false);
					GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(false);

					SetID(player->GetPlayerID());

					//tell powerup manager it was picked up
					PowerUpEvent* powerEvent = new PowerUpEvent(GetGameObject()->GetName(), true, spawnIndex, posIndex);
					powerEvent->SetClientID(player->GetPlayerID());
					EventDispatcher::GetSingleton()->DispatchTo(powerEvent, "PowerUpManager");
					delete powerEvent;

					//play sound
					SoundEvent* soundEvent = new SoundEvent();
					soundEvent->Init(AK::EVENTS::PLAY_FASTERLIGHTNING, GetGameObject()->FindIndexOfGameObject(GetGameObject()));
					EventDispatcher::GetSingleton()->DispatchTo(soundEvent, "Game");
					delete soundEvent;
				}

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
	isActivated = false;
}

void PowerUp::Enable()
{
	GetGameObject()->GetComponent<Renderer>()->SetEnabled(true);
	SetEnabled(true);
	GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(true);
}

void PowerUp::Disable()
{
	GetGameObject()->GetComponent<Renderer>()->SetEnabled(false);
	SetEnabled(false);
	GetGameObject()->GetComponent<SphereCollider>()->SetEnabled(false);
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

int PowerUp::GetSpawnIndex()
{
	return spawnIndex;
}

int PowerUp::GetPosIndex()
{
	return posIndex;
}

//setters//
void PowerUp::SetDuration(float dur)
{
	duration = dur;
}

void PowerUp::SetSpawnIndex(int index)
{
	spawnIndex = index;
}

void PowerUp::SetPosIndex(int index)
{
	posIndex = index;
}

void PowerUp::SetPlayer(PlayerController* playerC)
{
	player = playerC;
}