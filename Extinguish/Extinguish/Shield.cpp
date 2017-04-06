#include "Shield.h"
#include "PlayerController.h"
#include "GameObject.h"
#include "Scene.h"
#include "SphereCollider.h"
#include "Renderer.h"
#include "DeviceResources.h"

//structors
Shield::Shield(Scene* scene, XMFLOAT4X4 projection, DeviceResources* devResources)
{
	SetDuration(9.0f);

	//create an object that will look like a bubble around the player
	shieldBubble = new GameObject();
	shieldBubble->Init("ShieldBubble");
	scene->AddGameObject(shieldBubble);
	Renderer* renderer = new Renderer();
	shieldBubble->AddComponent(renderer);
	renderer->Init("ShieldBubble", "NormalMapped", "TempStatic", "", "", projection, devResources, true);
	renderer->SetEnabled(false);
	collider = new SphereCollider(1.0f, shieldBubble, false);
	shieldBubble->AddSphereCollider(collider);
	collider->SetOffset({ 0, 1.1f, 0 });
	//shieldBubble->AddSphereCollider(collider);
}

//basic
void Shield::Activate()
{
	GetPlayer()->SetIsInvincible(true);

	//attach collider to player
	//GetPlayer()->GetGameObject()->AddSphereCollider(collider);
	//collider->SetGameObject(GetPlayer()->GetGameObject());
	shieldBubble->GetTransform()->SetParent(GetPlayer()->GetGameObject()->GetTransform());
	collider->SetEnabled(true);
	shieldBubble->GetComponent<Renderer>()->SetEnabled(true);

	//disable ball collider
	GameObject* ball = GetGameObject()->FindGameObject("GameBall");
	ball->GetComponent<SphereCollider>()->SetEnabled(false);
	//ball->RemoveSphereCollider(collider);
	//ball->RemoveComponent<SphereCollider>();

	//shieldBubble->RemoveSphereCollider(collider);
	//shieldBubble->RemoveComponent<SphereCollider>();
}

void Shield::Deactivate()
{
	PowerUp::Deactivate();

	GetPlayer()->SetIsInvincible(false);

	//detach collider from player
	//shieldBubble->AddSphereCollider(collider);
	//GetPlayer()->GetGameObject()->RemoveSphereCollider(collider);
	//GetPlayer()->GetGameObject()->RemoveComponent<SphereCollider>();
	collider->SetEnabled(false);

	//enable ball collider
	GameObject* ball = GetGameObject()->FindGameObject("GameBall");
	ball->GetComponent<SphereCollider>()->SetEnabled(true);
}

void Shield::Enable()
{
	PowerUp::Enable();

	//shieldBubble->GetComponent<Renderer>()->SetEnabled(true);
	//collider->SetEnabled(true);
}

void Shield::Disable()
{
	PowerUp::Disable();

	//shieldBubble->GetComponent<Renderer>()->SetEnabled(false);
	//collider->SetEnabled(false);
}

//getters//
std::string Shield::GetName()
{
	return name;
}