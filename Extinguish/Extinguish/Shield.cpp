#include "Shield.h"
#include "PlayerController.h"
#include "GameObject.h"
#include "Scene.h"
#include "SphereCollider.h"
#include "ShieldRender.h"
#include "DeviceResources.h"
#include "Physics.h"
#include "SoundEngine.h"
#include "SoundEvent.h"

//structors
Shield::Shield(Scene* scene, XMFLOAT4X4 projection, DeviceResources* devResources)
{
	SetDuration(9.0f);

	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());

	//create an object that will look like a bubble around the player
	shieldBubble = new GameObject();
	shieldBubble->Init("ShieldBubble");
	shieldBubble->InitTransform(identity, { 0, 0, 0 }, { 0, 0, 0 }, { 9, 9, 9 }, nullptr, nullptr, nullptr);
	scene->AddGameObject(shieldBubble);
	ShieldRender* renderer = new ShieldRender(shieldBubble);
	shieldBubble->AddComponent(renderer);
	renderer->Init("ShieldBubble", projection, devResources, 0.1f, 0.56f);
	renderer->SetShieldColor(float4(0.9f, 0.9f, 0, 1));
	renderer->SetEnabled(false);
	collider = new SphereCollider(5.0f, shieldBubble, false);
	shieldBubble->AddSphereCollider(collider);
	collider->SetOffset({ 0, 1.1f, 0 });
	collider->IgnoreHex(true);
	collider->IgnoreSphere(true);
	collider->SetEnabled(false);
	Physics* physics = new Physics();
	shieldBubble->AddComponent(physics);
	physics->Init();
	physics->SetIsKinematic(true);
}

//basic
void Shield::Activate()
{
	GetPlayer()->SetIsInvincible(true);

	//attach shield buble to player
	shieldBubble->GetTransform()->SetParent(GetPlayer()->GetGameObject()->GetTransform());
	collider->IgnoreGameObject(GetPlayer()->GetGameObject());
	collider->SetEnabled(true);
	shieldBubble->GetComponent<ShieldRender>()->SetEnabled(true);
	shieldBubble->GetTransform()->SetPosition({ 0.0f, 0.0f, 0.0f });

	//play shield bubble efect
	SoundEvent* soundEvent = new SoundEvent();
	soundEvent->Init(AK::EVENTS::PLAY_FORCEFIELD, GetGameObject()->FindIndexOfGameObject(GetGameObject()));
	EventDispatcher::GetSingleton()->DispatchTo(soundEvent, "Game");
	delete soundEvent;
}

void Shield::Deactivate()
{
	PowerUp::Deactivate();

	GetPlayer()->SetIsInvincible(false);

	//detach shield bubble from player
	GetPlayer()->GetGameObject()->GetTransform()->RemoveChild(GetGameObject()->GetTransform());
	collider->SetEnabled(false);
	collider->UnIgnoreGameObject(GetPlayer()->GetGameObject());
	shieldBubble->GetComponent<ShieldRender>()->SetEnabled(false);

	//stop shield bubble sound effect
	SoundEvent* soundEvent = new SoundEvent();
	soundEvent->Init(AK::EVENTS::STOP_FORCEFIELD, GetGameObject()->FindIndexOfGameObject(GetGameObject()));
	EventDispatcher::GetSingleton()->DispatchTo(soundEvent, "Game");
	delete soundEvent;
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

//collision
void Shield::OnCollisionEnter(Collider* collider)
{
	int num = 69;

	num++;
}

//getters//
std::string Shield::GetName()
{
	return name;
}