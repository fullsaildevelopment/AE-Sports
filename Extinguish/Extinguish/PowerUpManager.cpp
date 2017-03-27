#include "PowerUpManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "PowerUp.h"
#include "SuperJump.h"
#include "BoxCollider.h"
#include "Magnet.h"
#include "Shield.h"

void PowerUpManager::Init(Scene* scene, XMFLOAT4X4& projection, DeviceResources* devResources)
{
	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, DirectX::XMMatrixIdentity());

	GameObject* superJump = new GameObject();
	superJump->Init("Super Jump");
	scene->AddGameObject(superJump);
	superJump->InitTransform(identity, { 5, 1, 5 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* superJumpRenderer = new Renderer();
	superJump->AddComponent(superJumpRenderer);
	superJumpRenderer->Init("Super Jump", "PowerUp", "NormalMapped", "TempStatic", "", "", projection, devResources, true);
	SuperJump* superJumpC = new SuperJump();
	superJump->AddComponent(superJumpC);
	BoxCollider* superJumpCollider = new BoxCollider(superJump, true, { 0.5f, 0.5f, 0.1f }, { -0.5f, -0.5f, -0.1f });
	superJump->AddBoxCollider(superJumpCollider);

	powerUps.push_back(superJumpC);

	GameObject* magnet = new GameObject();
	magnet->Init("Magnet");
	scene->AddGameObject(magnet);
	magnet->InitTransform(identity, { 5, 1, 3 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* magnetRenderer = new Renderer();
	magnet->AddComponent(magnetRenderer);
	magnetRenderer->Init("Magnet", "PowerUp", "NormalMapped", "TempStatic", "", "", projection, devResources, true);
	Magnet* magnetC = new Magnet();
	magnet->AddComponent(magnetC);
	BoxCollider* magnetCollider = new BoxCollider(magnet, true, { 0.5f, 0.5f, 0.1f }, { -0.5f, -0.5f, -0.1f });
	magnet->AddBoxCollider(magnetCollider);

	powerUps.push_back(magnetC);

	GameObject* shield = new GameObject();
	shield->Init("Shield");
	scene->AddGameObject(shield);
	shield->InitTransform(identity, { 5, 1, 5 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* shieldRenderer = new Renderer();
	shield->AddComponent(shieldRenderer);
	shieldRenderer->Init("Shield", "PowerUp", "NormalMapped", "TempStatic", "", "", projection, devResources, true);
	SuperJump* shieldC = new SuperJump();
	shield->AddComponent(shieldC);
	BoxCollider* shieldCollider = new BoxCollider(shield, true, { 0.5f, 0.5f, 0.1f }, { -0.5f, -0.5f, -0.1f });
	shield->AddBoxCollider(shieldCollider);

	powerUps.push_back(shieldC);
}