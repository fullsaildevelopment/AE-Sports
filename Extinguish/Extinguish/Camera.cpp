#include "Camera.h"

using namespace DirectX;

//basic//
void Camera::Init(XMVECTORF32 eye, XMVECTORF32 at, XMVECTORF32 up, float moveVel, float rotateVel)
{
	//set camera initial position
	XMFLOAT4X4 tempCamera;
	XMStoreFloat4x4(&tempCamera, XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up)));
	GetGameObject()->GetTransform()->SetLocal(tempCamera);

	moveSpeed = moveVel;
	rotateSpeed = rotateVel;

	//cache
	transform = GetGameObject()->GetTransform();
}

void Camera::Update(float dt, InputManager* input)
{
	this->input = input;

	//ClampTo();
	MoveCamera(dt);
}

//private helper functions

void Camera::MoveCamera(float dt)
{
	XMFLOAT4X4 camera = transform->GetWorld();

	if (input->GetKey('I'))
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, moveSpeed * dt);
		XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
		XMMATRIX newCamera = XMMatrixMultiply(translation, tempCamera);
		XMStoreFloat4x4(&camera, newCamera);
	}

	if (input->GetKey('K'))
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, -moveSpeed * dt);
		XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
		XMMATRIX newCamera = XMMatrixMultiply(translation, tempCamera);
		XMStoreFloat4x4(&camera, newCamera);
	}

	if (input->GetKey('J'))
	{
		XMMATRIX translation = XMMatrixTranslation(-moveSpeed * dt, 0.0f, 0.0f);
		XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
		XMMATRIX newCamera = XMMatrixMultiply(translation, tempCamera);
		XMStoreFloat4x4(&camera, newCamera);
	}

	if (input->GetKey('L'))
	{
		XMMATRIX translation = XMMatrixTranslation(moveSpeed * dt, 0.0f, 0.0f);
		XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
		XMMATRIX newCamera = XMMatrixMultiply(translation, tempCamera);
		XMStoreFloat4x4(&camera, newCamera);
	}

	if (input->GetKey('U')) //up
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, moveSpeed * dt, 0.0f);
		XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
		XMMATRIX newCamera = XMMatrixMultiply(translation, tempCamera);
		XMStoreFloat4x4(&camera, newCamera);
	}

	if (input->GetKey('P')) //down
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, -moveSpeed * dt, 0.0f);
		XMMATRIX tempCamera = XMLoadFloat4x4(&camera);
		XMMATRIX newCamera = XMMatrixMultiply(translation, tempCamera);
		XMStoreFloat4x4(&camera, newCamera);
	}

#if _DEBUG
	if (input->GetMouseX() && input->GetMouseY())
	{
		if (input->GetMouseButton(1) && prevMouseX && prevMouseY)
		{
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
		}

		prevMouseX = input->GetMouseX();
		prevMouseY = input->GetMouseY();
	}
#endif

	transform->SetLocal(camera);
}

void Camera::ClampTo()
{
	clamp._41 += 5.0f;
	clamp._42 += 5.0f;
	clamp._43 += 5.0f;

	//transform->SetLocal(clamp);
}

//setters//
void Camera::SetClampMatrix(DirectX::XMFLOAT4X4 matrix)
{
	clamp = matrix;
}