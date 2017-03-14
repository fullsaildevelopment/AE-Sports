#include "Camera.h"
#include <iostream>
#include "EventDispatcher.h"
#include "GameObject.h"
#include "GamePadEvent.h"
#include "GamePad.h"
#include "Game.h"

using namespace DirectX;
using namespace std;

Camera::Camera() : maxRotX(1.5f), maxRotY(2.0f), padSensitivityX(2.5f), padSensitivityY(2.5f)
{
	//camTransform = new Transform();
}

Camera::~Camera()
{
	//delete camTransform;
}

//basic//
void Camera::Init(XMVECTORF32 eye, XMVECTORF32 at, XMVECTORF32 up, float moveVel, float rotateVel, bool isEventHandler)
{
	//cache
	transform = GetGameObject()->GetTransform();
	input = InputManager::GetSingleton();

	//register handler
	if (isEventHandler)
	{
		EventDispatcher::GetSingleton()->RegisterHandler(this, GetGameObject()->GetName());
	}

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

void Camera::Update(float _dt)
{
	//this->input = input;
	this->_dt = _dt;
}

void Camera::HandleEvent(Event* e)
{
	//filter throw events to find right one
	InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

	if (inputDownEvent)
	{
		if (GetGameObject()->GetTransform()->GetParent()) {
			//if (inputDownEvent->IsServer())
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

		return;
	}

	GamePadEvent* gamePadEvent = dynamic_cast<GamePadEvent*>(e);

	if (gamePadEvent)
	{
		string name;
		name = "Camera";
		name += to_string(gamePadEvent->GetClientID());

		if (GetGameObject()->GetName() == name)
		{
			string playerName = "Mage";
			playerName += to_string(gamePadEvent->GetClientID());
			playerTransform = GetGameObject()->FindGameObject(playerName)->GetTransform();

			MoveCamera(gamePadEvent);
		}

		return;
	}
}

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
			curRotX = transform->GetRotation().x;

			float dx = (float)input->GetMouseX() - (float)prevMouseX;
			float dy = (float)input->GetMouseY() - (float)prevMouseY;

			//store old cam position
			float radX = dy * rotateSpeed * _dt;
			float radY = dx * rotateSpeed * _dt;


			//limit camera's rotation on x-axis
			if (curRotX + radX > maxRotX)
			{
				curRotX = maxRotX;
				radX = maxRotX - curRotX;
			}
			else if (curRotX + radX < -maxRotX)
			{
				curRotX = -maxRotX;
				radX = -maxRotX - curRotX;
			}
			else
			{
				curRotX += radX;
			}

			//curRotY += radY;

			transform->RotateX(radX); //only camera can rotate on the x
			playerTransform->RotateY(radY); //this rotates camera as well given its a child to player

			//cout << curRotX << endl;
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

void Camera::MoveCamera(GamePadEvent* e)
{
	GamePad::State* padState = e->GetState();

	if (padState->thumbSticks.rightX || padState->thumbSticks.rightY)
	{
		//if (!input->GetMouseButton(0) && prevMouseX && prevMouseY)
		//if (input->GetMouseButton(2) && prevMouseX && prevMouseY)
		if (!padState->IsLeftTriggerPressed())
		{
			XMFLOAT4X4 camera = transform->GetWorld();

			float dx = padState->thumbSticks.rightX;
			float dy = -padState->thumbSticks.rightY;

			//store old cam position
			float degX = dy * rotateSpeed * _dt * 2.5f;
			float degY = dx * rotateSpeed * _dt * 2.5f;


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
			playerTransform->RotateY(degY);
		}
	}
}

//getters//
XMFLOAT4X4 Camera::GetView()
{
	XMFLOAT4X4 view;

	//XMStoreFloat4x4(&view, XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up )));
	//XMFLOAT3 translation = { 0, 5, 1 };
	//transform->Translate({ translation.x, translation.y, translation.z });
	//transform->RotateY(XM_PI);
	XMStoreFloat4x4(&view, XMMatrixInverse(nullptr, XMLoadFloat4x4(&transform->GetWorld())));
	//transform->Translate({ -translation.x, -translation.y, -translation.z });
	//transform->RotateY(-XM_PI);

	return view;
}