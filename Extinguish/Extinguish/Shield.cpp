#include "Shield.h"
#include "PlayerController.h"
#include "GameObject.h"
#include "Scene.h"
#include "SphereCollider.h"
#include "Renderer.h"

//structors
Shield::Shield(Scene* scene)
{
	SetDuration(9.0f);

	//create an object that will look like a bubble around the player
	shieldBubble = new GameObject();
	shieldBubble->Init("ShieldBubble");
	scene->AddGameObject(shieldBubble);
	//Renderer* renderer = new Renderer();
	//shieldBubble->AddComponent(renderer);
	//renderer->Init(
	collider = new SphereCollider(5.0f, shieldBubble, false);
	shieldBubble->AddSphereCollider(collider);
}

//basic
void Shield::Activate()
{
	GetPlayer()->SetIsInvincible(true);

	//attach collider to player
	GetPlayer()->GetGameObject()->AddSphereCollider(collider);
	shieldBubble->RemoveSphereCollider(collider);
	shieldBubble->RemoveComponent<SphereCollider>();
}

void Shield::Deactivate()
{
	PowerUp::Deactivate();

	GetPlayer()->SetIsInvincible(false);

	//detach collider from player
	shieldBubble->AddSphereCollider(collider);
	GetPlayer()->GetGameObject()->RemoveSphereCollider(collider);
	GetPlayer()->GetGameObject()->RemoveComponent<SphereCollider>();
}

void Shield::Enable()
{
	PowerUp::Enable();

	//shieldBubble->GetComponent<Renderer>()->SetEnabled(true);
	collider->SetEnabled(true);
}

void Shield::Disable()
{
	PowerUp::Disable();

	//shieldBubble->GetComponent<Renderer>()->SetEnabled(false);
	collider->SetEnabled(false);
}

//getters//
std::string Shield::GetName()
{
	return name;
}