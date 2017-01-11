#include "PlayerController.h"
#include "Camera.h"

PlayerController::PlayerController()
{

}

PlayerController::~PlayerController()
{

}

void PlayerController::Init()
{
	//cache
	transform = GetGameObject()->GetTransform();
	cameraTransform = GetGameObject()->FindGameObject("Camera")->GetTransform();
	camera = GetGameObject()->FindGameObject("Camera")->GetComponent<Camera>();;
	//camera->GetGameObject()->GetTransform()->SetPosition(transform->GetPosition());
}

void PlayerController::Update(float dt, InputManager* input)
{
	this->input = input;

	//react to input to move character
	MovePlayer(dt);

	//give camera world
	//camera->SetClampPos(transform->GetPosition());

	//ClampToCamera();

	//GetGameObject()->GetTransform()->Translate({ 0.01f, 0, 0 });
	//GetGameObject()->GetTransform()->RotateY(0.005f);
}

//private helper functions
void PlayerController::MovePlayer(float dt)
{
	

//#if _DEBUG
//	if (input->GetMouseX() && input->GetMouseY())
//	{
//		if (input->GetMouseButton(1) && prevMouseX && prevMouseY)
//		{
//			XMFLOAT4X4 player = transform->GetWorld();
//
//			float dx = (float)input->GetMouseX() - (float)prevMouseX;
//			float dy = (float)input->GetMouseY() - (float)prevMouseY;
//
//			//store old cam position
//			XMFLOAT3 camPosition = XMFLOAT3(player._41, player._42, player._43);
//
//			player._41 = 0;
//			player._42 = 0;
//			player._43 = 0;
//
//			XMMATRIX rotX = XMMatrixRotationX(dy * rotateSpeed * dt);
//			XMMATRIX rotY = XMMatrixRotationY(dx * rotateSpeed * dt);
//
//			//apply rotations to player
//			XMMATRIX tempPlayer = XMLoadFloat4x4(&player);
//			tempPlayer = XMMatrixMultiply(rotX, tempPlayer);
//			tempPlayer = XMMatrixMultiply(tempPlayer, rotY);
//
//			//store new player
//			XMStoreFloat4x4(&player, tempPlayer);
//
//			//change position to where it was earlier
//			player._41 = camPosition.x;
//			player._42 = camPosition.y;
//			player._43 = camPosition.z;
//
//			transform->SetLocal(player);
//			camera->UpdateCamsRotation(dy * rotateSpeed * dt, dx * rotateSpeed * dt);
//		}
//
//		prevMouseX = input->GetMouseX();
//		prevMouseY = input->GetMouseY();
//	}
//#else
//	if (input->GetMouseX() && input->GetMouseY())
//	{
//		if (prevMouseX && prevMouseY)
//		{
//			XMFLOAT4X4 player = transform->GetWorld();
//
//			float dx = (float)input->GetMouseX() - (float)prevMouseX;
//			float dy = (float)input->GetMouseY() - (float)prevMouseY;
//
//			//store old cam position
//			XMFLOAT3 camPosition = XMFLOAT3(player._41, player._42, player._43);
//
//			player._41 = 0;
//			player._42 = 0;
//			player._43 = 0;
//
//			XMMATRIX rotX = XMMatrixRotationX(dy * rotateSpeed * dt);
//			XMMATRIX rotY = XMMatrixRotationY(dx * rotateSpeed * dt);
//
//			//apply rotations to player
//			XMMATRIX tempPlayer = XMLoadFloat4x4(&player);
//			tempPlayer = XMMatrixMultiply(rotX, tempPlayer);
//			tempPlayer = XMMatrixMultiply(tempPlayer, rotY);
//
//			//store new player
//			XMStoreFloat4x4(&player, tempPlayer);
//
//			//change position to where it was earlier
//			player._41 = camPosition.x;
//			player._42 = camPosition.y;
//			player._43 = camPosition.z;
//
//			transform->SetLocal(player);
//		}
//
//		prevMouseX = input->GetMouseX();
//		prevMouseY = input->GetMouseY();
//	}
//#endif
}

//void PlayerController::ClampToCamera()
//{
//	GetGameObject()->GetTransform()->SetLocal(camera);
//}
