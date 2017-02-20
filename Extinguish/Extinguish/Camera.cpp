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

	// init sensitivities... make more dynamic in future through options if we want to be cool
#if _DEBUG
	sensitivityX = 1.0f;
	sensitivityY = 1.0f;
#else
	sensitivityX = 3.5f;
	sensitivityY = 3.5f;
#endif
}

void Camera::Update(float dt)
{
	//this->input = input;
	this->dt = dt;
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

//void Camera::UpdateCamsRotation(float x, float y)
//{
//	XMFLOAT4X4 world = transform->GetWorld();
//
//	world._41 = 0;
//	world._42 = 0;
//	world._43 = 0;
//
//	XMMATRIX rotX = XMMatrixRotationX(x * rotateSpeed);
//	XMMATRIX rotY = XMMatrixRotationY(y * rotateSpeed);
//
//	XMMATRIX tempView = XMLoadFloat4x4(&world);
//	tempView = XMMatrixMultiply(rotX, tempView);
//	tempView = XMMatrixMultiply(tempView, rotY);
//	XMStoreFloat4x4(&world, tempView);
//
//	//change position to where it was earlier
//	world._41 = transform->GetPosition().x;
//	world._42 = transform->GetPosition().y;
//	world._43 = transform->GetPosition().z;
//
//	transform->SetLocal(world);
//}

//private helper functions

void Camera::MoveCamera(InputDownEvent* e)
{
	input = e->GetInput();

#if _DEBUG
	if (input->GetMouseX() && input->GetMouseY())
	{
		//if (!input->GetMouseButton(0) && prevMouseX && prevMouseY)
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

			transform->RotateX(degX);
			//playerTransform->GetChild(1)->RotateX(degX); //rotate crosse as well
			playerTransform->RotateY(degY);
		}
	}

	prevMouseX = input->GetMouseX();
	prevMouseY = input->GetMouseY();
#else
	if (input->GetMouseX() && input->GetMouseY() && input->GetMouseButton(2))
	{
		if (prevMouseX && prevMouseY)
		{
			XMFLOAT4X4 camera = transform->GetWorld();

			float dx = (float)input->GetMouseX() - (float)prevMouseX;
			float dy = (float)input->GetMouseY() - (float)prevMouseY;

			//store old cam position
			float degX = dy * rotateSpeed * dt * sensitivityX;
			float degY = dx * rotateSpeed * dt * sensitivityY;

			cout << dt << endl;

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
			//playerTransform->GetChild(1)->RotateX(degX); //rotate crosse as well
			playerTransform->RotateY(degY);
		}
	}

	prevMouseX = input->GetMouseX();
	prevMouseY = input->GetMouseY();
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
	//transform->Translate({ translation.x, translation.y, translation.z });
	//transform->RotateY(XM_PI);
	XMStoreFloat4x4(&view, XMMatrixInverse(nullptr, XMLoadFloat4x4(&transform->GetWorld())));
	//transform->Translate({ -translation.x, -translation.y, -translation.z });
	//transform->RotateY(-XM_PI);

	return view;
}