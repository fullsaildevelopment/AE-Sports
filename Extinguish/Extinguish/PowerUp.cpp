#include "PowerUp.h"
#include "Collider.h"
#include "CapsuleCollider.h"
#include "GameObject.h"
#include "PlayerController.h"
#include "Renderer.h"

//basic//
void PowerUp::Update(float _dt)
{
	if (IsDone())
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
	if (collider->GetColliderType() == Collider::ColliderType::CTCapsule)
	{
		CapsuleCollider* capsCollider = (CapsuleCollider*)collider;

		if (capsCollider->GetGameObject()->GetName().find("Mage") != string::npos)
		{
			player = capsCollider->GetGameObject()->GetComponent<PlayerController>();

			Activate();

			//stop rendering
			capsCollider->GetGameObject()->GetComponent<Renderer>()->SetEnabled(false);

			return;
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
}

//getters//
//float PowerUp::GetCooldown()
//{
//	return cooldown;
//}

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

//setters//
//void PowerUp::SetCooldown(float down)
//{
//	cooldown = down;
//}

void PowerUp::SetDuration(float dur)
{
	duration = dur;
}