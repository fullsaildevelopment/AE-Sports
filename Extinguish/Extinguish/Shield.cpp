#include "Shield.h"
#include "PlayerController.h"
#include "GameObject.h"
#include "Scene.h"
#include "SphereCollider.h"
#include "Renderer.h"
#include "DeviceResources.h"
#include "Physics.h"

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
	Renderer* renderer = new Renderer();
	shieldBubble->AddComponent(renderer);
	renderer->Init("ShieldBubble", "NormalMapped", "TempStatic", "", "", projection, devResources, true);
	renderer->SetEmissiveColor(float4(0, 0, 0.5f, 1));
	renderer->SetEnabled(false);
	collider = new SphereCollider(5.0f, shieldBubble, false);
	shieldBubble->AddSphereCollider(collider);
	collider->SetOffset({ 0, 1.1f, 0 });
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
	collider->SetEnabled(true);
	shieldBubble->GetComponent<Renderer>()->SetEnabled(true);
	shieldBubble->GetTransform()->SetPosition({ 0.0f, 0.0f, 0.0f });
}

void Shield::Deactivate()
{
	PowerUp::Deactivate();

	GetPlayer()->SetIsInvincible(false);

	//detach shield bubble from player
	GetPlayer()->GetGameObject()->GetTransform()->RemoveChild(GetGameObject()->GetTransform());
	collider->SetEnabled(false);
	shieldBubble->GetComponent<Renderer>()->SetEnabled(false);
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