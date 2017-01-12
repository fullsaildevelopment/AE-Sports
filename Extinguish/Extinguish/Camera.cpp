#include "Camera.h"
#include <iostream>

using namespace DirectX;
using namespace std;

Camera::Camera() : maxRotX(1.0f), maxRotY(1.5f)
{

}

Camera::~Camera()
{

}

//basic//
void Camera::Init(XMVECTORF32 eye, XMVECTORF32 at, XMVECTORF32 up, float moveVel, float rotateVel)
{
	//cache
	transform = GetGameObject()->GetTransform();

	//set camera initial position
	XMStoreFloat4x4(&view, XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up)));
	transform->SetLocal(view);

	moveSpeed = moveVel;
	rotateSpeed = rotateVel;
}

void Camera::Update(float dt, InputManager* input)
{
	this->input = input;

	//ClampTo();
	MoveCamera(dt);
	//transform->RotateY(0.001f);
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

void Camera::MoveCamera(float dt)
{
	if (input->GetKey('W'))
	{
		transform->Translate({ 0.0f, 0.0f, moveSpeed * dt });
	}

	if (input->GetKey('S'))
	{
		transform->Translate({ 0.0f, 0.0f, -moveSpeed * dt });
	}

	if (input->GetKey('A'))
	{
		transform->Translate({ -moveSpeed * dt, 0.0f, 0.0f });
	}

	if (input->GetKey('D'))
	{
		transform->Translate({ moveSpeed * dt, 0.0f, 0.0f });
	}

	if (input->GetKey('Q')) //up
	{
		transform->Translate({ 0.0f, moveSpeed * dt, 0.0f });
	}

	if (input->GetKey('E')) //down
	{
		transform->Translate({ 0.0f, -moveSpeed * dt, 0.0f });
	}

#if _DEBUG
	if (input->GetMouseX() && input->GetMouseY())
	{
		if (input->GetMouseButton(1) && prevMouseX && prevMouseY)
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

			cout << curRotX << " " << curRotY << endl;

			XMMATRIX rotX = XMMatrixRotationX(degX);
			XMMATRIX rotY = XMMatrixRotationY(degY);

			//apply rotations to camera

			XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
			tempCamera = XMMatrixMultiply(rotX, tempCamera);
			tempCamera = XMMatrixMultiply(tempCamera, rotY);

			//store new camera
			XMStoreFloat4x4(&camera, tempCamera);

			//change position to where it was earlier
			camera._41 = camPosition.x;
			camera._42 = camPosition.y;
			camera._43 = camPosition.z;

			transform->SetLocal(camera);
		}

		prevMouseX = input->GetMouseX();
		prevMouseY = input->GetMouseY();
	}
#else
	if (input->GetMouseX() && input->GetMouseY())
	{
		if (prevMouseX && prevMouseY)
		{
			XMFLOAT4X4 camera = transform->GetWorld();

			float dx = (float)input->GetMouseX() - (float)prevMouseX;
			float dy = (float)input->GetMouseY() - (float)prevMouseY;

			//store old cam position
			XMFLOAT3 camPosition = XMFLOAT3(camera._41, camera._42, camera._43);

			camera._41 = 0;
			camera._42 = 0;
			camera._43 = 0;

			XMMATRIX rotX = XMMatrixRotationX(dy * rotateSpeed * dt);
			XMMATRIX rotY = XMMatrixRotationY(dx * rotateSpeed * dt);

			//apply rotations to camera
			XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
			tempCamera = XMMatrixMultiply(rotX, tempCamera);
			tempCamera = XMMatrixMultiply(tempCamera, rotY);

			//store new camera
			XMStoreFloat4x4(&camera, tempCamera);

			//change position to where it was earlier
			camera._41 = camPosition.x;
			camera._42 = camPosition.y;
			camera._43 = camPosition.z;

			transform->SetLocal(camera);
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

	transform->SetPosition({ clampPos.x, clampPos.y + 3.5f, clampPos.z + 0.5f});
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
	XMStoreFloat4x4(&view, XMMatrixInverse(nullptr, XMLoadFloat4x4(&transform->GetWorld())));

	return view;
}