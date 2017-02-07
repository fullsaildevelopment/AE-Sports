#include "Camera.h"
#include <iostream>
#include "EventDispatcher.h"
#include "GameObject.h"

using namespace DirectX;
using namespace std;

Camera::Camera() : maxRotX(1.5f), maxRotY(2.0f)
{
	//camTransform = new Transform();
}

Camera::~Camera()
{
	//delete camTransform;
}

//basic//
void Camera::Init(XMVECTORF32 eye, XMVECTORF32 at, XMVECTORF32 up, float moveVel, float rotateVel)
{
	//cache
	transform = GetGameObject()->GetTransform();
	input = InputManager::GetSingleton();

	//register handler
	EventDispatcher::GetSingleton()->RegisterHandler(this, GetGameObject()->GetName());

	//set camera initial position
	XMStoreFloat4x4(&view, XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up)));
	transform->SetLocal(view);

	moveSpeed = moveVel;
	rotateSpeed = rotateVel;
}

void Camera::Update(float dt)
{
	//this->input = input;
	this->dt = dt;

	//ClampTo();
	//MoveCamera();
	//transform->RotateY(0.001f);
}

void Camera::HandleEvent(Event* e)
{
	//filter throw events to find right one
	InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

	if (inputDownEvent)
	{
		if (GetGameObject()->GetTransform()->GetParent()) {
			if (inputDownEvent->IsServer())
			{
				string name;
				name = "Camera";
				name += to_string(inputDownEvent->GetID());

				if (GetGameObject()->GetName() == name)
				{
					string playerName = "Mage";
					playerName += to_string(inputDownEvent->GetID());

					playerTransform = GetGameObject()->FindGameObject(playerName)->GetTransform();
					MoveCamera(inputDownEvent);
				}
			}
		}
	}
}

void Camera::UpdateCamsRotation(float x, float y)
{
	XMFLOAT4X4 world = transform->GetWorld();

	world._41 = 0;
	world._42 = 0;
	world._43 = 0;

	XMMATRIX rotX = XMMatrixRotationX(x * rotateSpeed);
	XMMATRIX rotY = XMMatrixRotationY(y * rotateSpeed);

	XMMATRIX tempView = XMLoadFloat4x4(&world);
	tempView = XMMatrixMultiply(rotX, tempView);
	tempView = XMMatrixMultiply(tempView, rotY);
	XMStoreFloat4x4(&world, tempView);

	//change position to where it was earlier
	world._41 = transform->GetPosition().x;
	world._42 = transform->GetPosition().y;
	world._43 = transform->GetPosition().z;

	transform->SetLocal(world);
}

//private helper functions

void Camera::MoveCamera(InputDownEvent* e)
{
	input = e->GetInput();

	//cout << "Input!" << endl;

	//if (input->GetKey('W'))
	//{
	//	XMFLOAT3 forward = transform->GetForward();
	//	//forward = { -forward.x, -forward.y, -forward.z };
	//	transform->Translate({ forward.x * moveSpeed * dt, forward.y * moveSpeed * dt,  forward.z * moveSpeed * dt });
	////	cout << "Forward!" << endl;
	//}

	//if (input->GetKey('S'))
	//{
	//	XMFLOAT3 forward = transform->GetForward();
	//	//forward = { -forward.x, -forward.y, -forward.z };
	//	transform->Translate({ forward.x * -moveSpeed * dt, forward.y * -moveSpeed * dt,  forward.z * -moveSpeed * dt });
	//}

	//if (input->GetKey('A'))
	//{
	//	XMFLOAT3 right = transform->GetRight();
	//	//right = { -right.x, -right.y, -right.z };
	//	transform->Translate({ right.x * -moveSpeed * dt, right.y * -moveSpeed * dt,  right.z * -moveSpeed * dt });
	//}

	//if (input->GetKey('D'))
	//{
	//	XMFLOAT3 right = transform->GetRight();
	//	//right = { -right.x, -right.y, -right.z };
	//	transform->Translate({ right.x * moveSpeed * dt, right.y * moveSpeed * dt,  right.z * moveSpeed * dt });
	//}

	//if (input->GetKey('Q')) //up
	//{
	//	XMFLOAT3 up = transform->GetUp();
	//	//up = { -up.x, -up.y, -up.z };
	//	transform->Translate({ up.x * moveSpeed * dt, up.y * moveSpeed * dt,  up.z * moveSpeed * dt });
	//}

	//if (input->GetKey('E')) //down
	//{
	//	XMFLOAT3 up = transform->GetUp();
	//	//up = { -up.x, -up.y, -up.z };
	//	transform->Translate({ up.x * -moveSpeed * dt, up.y * -moveSpeed * dt,  up.z * -moveSpeed * dt });
	//}

#if _DEBUG
	if (input->GetMouseX() && input->GetMouseY())
	{
		if (input->GetMouseButton(2) && prevMouseX && prevMouseY)
		{
			XMFLOAT4X4 camera = transform->GetWorld();

			float dx = (float)input->GetMouseX() - (float)prevMouseX;
			float dy = (float)input->GetMouseY() - (float)prevMouseY;

			//store old cam position
			float degX = dy * rotateSpeed * dt;
			float degY = dx * rotateSpeed * dt;


			if (curRotX + degX > maxRotX)
			{
				curRotX = maxRotX;
				degX = maxRotX - curRotX;
			}
			else if (curRotX + degX < -maxRotX)
			{
				curRotX = -maxRotX;
				degX = -maxRotX - curRotX;
			}
			else
			{
				curRotX += degX;
			}

			curRotY += degY;

			//cout << curRotX << " " << curRotY << endl;

			//transform->RotateX(degX);
			//transform->RotateY(degY);
			playerTransform->RotateY(degY);
		}
	}

	prevMouseX = input->GetMouseX();
	prevMouseY = input->GetMouseY();
#else
	if (input->GetMouseX() && input->GetMouseY())
	{
		if (prevMouseX && prevMouseY && !input->GetMouseButton(2))
		{
			XMFLOAT4X4 camera = transform->GetWorld();

			float dx = (float)input->GetMouseX() - (float)prevMouseX;
			float dy = (float)input->GetMouseY() - (float)prevMouseY;

			//store old cam position
			XMFLOAT3 camPosition = XMFLOAT3(camera._41, camera._42, camera._43);

			camera._41 = 0;
			camera._42 = 0;
			camera._43 = 0;

			float degX = dy * rotateSpeed * dt;
			float degY = dx * rotateSpeed * dt;

			if (curRotX + degX > maxRotX)
			{
				curRotX = maxRotX;
				degX = maxRotX - curRotX;
			}
			else if (curRotX + degX < -maxRotX)
			{
				curRotX = -maxRotX;
				degX = -maxRotX - curRotX;
			}
			else
			{
				curRotX += degX;
			}

			curRotY += degY;

			transform->RotateX(degX);
			transform->RotateY(degY);
		}

		prevMouseX = input->GetMouseX();
		prevMouseY = input->GetMouseY();
	}
#endif
}

void Camera::ClampTo()
{
	//clamp._41 += -10.0f;
	//clamp._42 += -10.0f;
	//clamp._43 += 5.0f;

	transform->SetPosition({ clampPos.x, clampPos.y + 3.5f, clampPos.z + 0.5f });
}

//setters//
//void Camera::SetClampMatrix(DirectX::XMFLOAT4X4 matrix)
//{
//	XMStoreFloat4x4(&clamp, XMMatrixInverse(nullptr, XMLoadFloat4x4(&matrix)));
//	//clamp = matrix;
//}

void Camera::SetClampPos(DirectX::XMFLOAT3 pos)
{
	clampPos = pos;
}


//getters//
XMFLOAT4X4 Camera::GetView()
{
	XMFLOAT4X4 view;

	//XMStoreFloat4x4(&view, XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up )));
	XMFLOAT3 translation = { 0, 5, 1 };
	transform->Translate({ translation.x, translation.y, translation.z });
	//transform->RotateY(XM_PI);
	XMStoreFloat4x4(&view, XMMatrixInverse(nullptr, XMLoadFloat4x4(&transform->GetWorld())));
	transform->Translate({ -translation.x, -translation.y, -translation.z });
	//transform->RotateY(-XM_PI);

	return view;
}