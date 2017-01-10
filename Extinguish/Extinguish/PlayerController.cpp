#include "PlayerController.h"

PlayerController::PlayerController()
{

}

PlayerController::~PlayerController()
{

}

void PlayerController::Init(float moveVelocity, float rotateVelocity)
{
	moveSpeed = moveVelocity;
	rotateSpeed = rotateVelocity;

	//cache
	transform = GetGameObject()->GetTransform();
}

void PlayerController::Update(float dt, InputManager* input)
{
	this->input = input;

	//react to input to move character
	//MovePlayer(dt);

	//give camera world
	//GetGameObject()->GetComponent<Camera>()->SetClampMatrix(transform->GetWorld());


	//ClampToCamera();

	//GetGameObject()->GetTransform()->Translate({ 0.01f, 0, 0 });
	//GetGameObject()->GetTransform()->RotateY(0.005f);
}

//private helper functions
void PlayerController::MovePlayer(float dt)
{
	XMFLOAT4X4 player = transform->GetWorld();

	if (input->GetKey('W'))
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, moveSpeed * dt);
		XMMATRIX tempPlayer = XMLoadFloat4x4(&player);
		XMMATRIX newPlayer = XMMatrixMultiply(translation, tempPlayer);
		XMStoreFloat4x4(&player, newPlayer);
	}

	if (input->GetKey('S'))
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, -moveSpeed * dt);
		XMMATRIX tempPlayer = XMLoadFloat4x4(&player);
		XMMATRIX newPlayer = XMMatrixMultiply(translation, tempPlayer);
		XMStoreFloat4x4(&player, newPlayer);
	}

	if (input->GetKey('A'))
	{
		XMMATRIX translation = XMMatrixTranslation(-moveSpeed * dt, 0.0f, 0.0f);
		XMMATRIX tempPlayer = XMLoadFloat4x4(&player);
		XMMATRIX newPlayer = XMMatrixMultiply(translation, tempPlayer);
		XMStoreFloat4x4(&player, newPlayer);
	}

	if (input->GetKey('D'))
	{
		XMMATRIX translation = XMMatrixTranslation(moveSpeed * dt, 0.0f, 0.0f);
		XMMATRIX tempPlayer = XMLoadFloat4x4(&player);
		XMMATRIX newPlayer = XMMatrixMultiply(translation, tempPlayer);
		XMStoreFloat4x4(&player, newPlayer);
	}

	if (input->GetKey('Q')) //up
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, moveSpeed * dt, 0.0f);
		XMMATRIX tempPlayer = XMLoadFloat4x4(&player);
		XMMATRIX newPlayer = XMMatrixMultiply(translation, tempPlayer);
		XMStoreFloat4x4(&player, newPlayer);
	}

	if (input->GetKey('E')) //down
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, -moveSpeed * dt, 0.0f);
		XMMATRIX tempPlayer = XMLoadFloat4x4(&player);
		XMMATRIX newPlayer = XMMatrixMultiply(translation, tempPlayer);
		XMStoreFloat4x4(&player, newPlayer);
	}

#if _DEBUG
	if (input->GetMouseX() && input->GetMouseY())
	{
		if (input->GetMouseButton(1) && prevMouseX && prevMouseY)
		{
			float dx = (float)input->GetMouseX() - (float)prevMouseX;
			float dy = (float)input->GetMouseY() - (float)prevMouseY;

			//store old cam position
			XMFLOAT3 camPosition = XMFLOAT3(player._41, player._42, player._43);

			player._41 = 0;
			player._42 = 0;
			player._43 = 0;

			XMMATRIX rotX = XMMatrixRotationX(dy * rotateSpeed * dt);
			XMMATRIX rotY = XMMatrixRotationY(dx * rotateSpeed * dt);

			//apply rotations to player
			XMMATRIX tempPlayer = XMLoadFloat4x4(&player);
			tempPlayer = XMMatrixMultiply(rotX, tempPlayer);
			tempPlayer = XMMatrixMultiply(tempPlayer, rotY);

			//store new player
			XMStoreFloat4x4(&player, tempPlayer);

			//change position to where it was earlier
			player._41 = camPosition.x;
			player._42 = camPosition.y;
			player._43 = camPosition.z;
		}

		prevMouseX = input->GetMouseX();
		prevMouseY = input->GetMouseY();
	}
#else
	if (input->GetMouseX() && input->GetMouseY())
	{
		if (prevMouseX && prevMouseY)
		{
			float dx = (float)input->GetMouseX() - (float)prevMouseX;
			float dy = (float)input->GetMouseY() - (float)prevMouseY;

			//store old cam position
			XMFLOAT3 camPosition = XMFLOAT3(player._41, player._42, player._43);

			player._41 = 0;
			player._42 = 0;
			player._43 = 0;

			XMMATRIX rotX = XMMatrixRotationX(dy * rotateSpeed * dt);
			XMMATRIX rotY = XMMatrixRotationY(dx * rotateSpeed * dt);

			//apply rotations to player
			XMMATRIX tempPlayer = XMLoadFloat4x4(&player);
			tempPlayer = XMMatrixMultiply(rotX, tempPlayer);
			tempPlayer = XMMatrixMultiply(tempPlayer, rotY);

			//store new player
			XMStoreFloat4x4(&player, tempPlayer);

			//change position to where it was earlier
			player._41 = camPosition.x;
			player._42 = camPosition.y;
			player._43 = camPosition.z;
		}

		prevMouseX = input->GetMouseX();
		prevMouseY = input->GetMouseY();
	}
#endif

	transform->SetLocal(player);
}

//void PlayerController::ClampToCamera()
//{
//	GetGameObject()->GetTransform()->SetLocal(camera);
//}
