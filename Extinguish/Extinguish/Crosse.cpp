#include "Crosse.h"

Crosse::Crosse()
{

}

Crosse::~Crosse()
{

}

void Crosse::Update(float dt, InputManager* input)
{
	HandleInput();

	//set crosse to be at player's position
	ClampToPlayer();
}

//private helper functions//

void Crosse::HandleInput()
{

}

void Crosse::ClampToPlayer()
{
	//get player's world
	Transform* playerTransform = GetGameObject()->GetTransform()->GetParent();

	//alter the world to suitable stick position
	XMFLOAT3 playerPos = playerTransform->GetPosition();
	playerPos.y += 100;
	playerPos.z -= 50;

	//set crosse's pos to pos
	GetGameObject()->GetTransform()->SetPosition(playerPos);
}