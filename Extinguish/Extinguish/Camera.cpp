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

	//moveTotalTime = -1;
	//lookTotalTime = -1;
}

void Camera::Update(float _dt)
{
	this->_dt = _dt;

//	lookCurTime += _dt;
//	moveCurTime += _dt;
//
//	//if (lerp)
//	//	LerpCamera(_dt);
//	//this->input = input;
//
//	if (moveTotalTime != -1)
//	{
//		Move();
//	}
//
//	if (lookTotalTime != -1)
//	{
//		Look();
//	}
}

void Camera::HandleEvent(Event* e)
{
	//filter throw events to find right one
	InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

	if (inputDownEvent)
	{
		if (GetGameObject()->GetTransform()->GetParent()/* && !lerp*/) {
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

//misc//
void Camera::LookAt(float3 pos, float totalTime)
{
	//lookDestination = pos;
	//lookTotalTime = totalTime;
	//lookCurTime = 0.0f;
}

void Camera::MoveTo(float3 pos, float totalTime)
{
	//moveDestination = pos;
	//moveTotalTime = totalTime;
	//moveCurTime = 0.0f;

	////detach camera from player
	//transform->SetPosition(transform->GetWorldPosition());

	////store position for lerp
	//originalPosition = transform->GetPosition();

	//transform->GetParent()->RemoveChild(transform);
	//transform->SetParent(nullptr);
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

//setters//
void Camera::SetDestination(float3 des)
{
	//destination = des;
	//destination.y = playerTransform->GetPosition().y;
}

//private helper functions
void Camera::MoveCamera(InputDownEvent* e)
{
	input = e->GetInput();

//#if _DEBUG
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
//#else
//	if (input->GetMouseX() && input->GetMouseY() && input->GetMouseButton(2))
//	{
//		if (prevMouseX && prevMouseY)
//		{
//			XMFLOAT4X4 camera = transform->GetWorld();
//
//			float dx = (float)input->GetMouseX() - (float)prevMouseX;
//			float dy = (float)input->GetMouseY() - (float)prevMouseY;
//
//			//store old cam position
//			float degX = dy * rotateSpeed * _dt * sensitivityX;
//			float degY = dx * rotateSpeed * _dt * sensitivityY;
//
//			cout << _dt << endl;
//
//			if (curRotX + degX > maxRotX)
//			{
//				curRotX = maxRotX;
//				degX = maxRotX - curRotX;
//			}
//
//			else if (curRotX + degX < -maxRotX)
//			{
//				curRotX = -maxRotX;
//				degX = -maxRotX - curRotX;
//			}
//
//			else
//			{
//				curRotX += degX;
//			}
//
//			curRotY += degY;
//
//			transform->RotateX(degX);
//			//playerTransform->GetChild(1)->RotateX(degX); //rotate crosse as well
//			playerTransform->RotateY(degY);
//		}
//	}
//
//	prevMouseX = input->GetMouseX();
//	prevMouseY = input->GetMouseY();
//#endif
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

void Camera::Move()
{
	//float ratio = moveCurTime / moveTotalTime;

	////in order to make camera move nicely, I might have to disattach it from the player

	//if (ratio <= 1.0f)
	//{
	//	float3 camPosition = transform->GetPosition();
	//	float3 newPosition;

	//	newPosition.x = (moveDestination.x - originalPosition.x) * ratio + originalPosition.x;
	//	newPosition.y = (moveDestination.y - originalPosition.y) * ratio + originalPosition.y;
	//	newPosition.z = (moveDestination.z - originalPosition.z) * ratio + originalPosition.z;

	//	transform->SetPosition(newPosition);

	//	if (GetGameObject()->GetName() == "Camera1")
	//	{
	//		int breakPoint = 69;
	//		breakPoint = breakPoint;
	//	}
	//}
	//else
	//{
	//	moveTotalTime = -1; //no more moving
	//}
}

void Camera::Look()
{

}

void Camera::LerpCamera(float dt)
{
	//if (lerp) {
	//	XMFLOAT4X4 camera = transform->GetWorld();
	//	XMFLOAT3 current;
	//	current = { camera._41, camera._42, camera._43 };

	//	curTime += dt;
	//	if (curTime > maxTime)
	//	{
	//		curTime = maxTime;
	//		lerp = false;
	//	}
	//	float ratio = curTime / maxTime;
	//	XMFLOAT3 newposition;

	//	newposition.x = (destination.x - current.x) * ratio + current.x;
	//	newposition.y = (destination.y - current.y) * ratio + current.y;
	//	newposition.z = (destination.z - current.z) * ratio + current.z;	

	//	float3 newpos(newposition.x, newposition.y, newposition.z);
	//	// rebuild camera
	//	transform->SetPosition(newpos);
	//}
	//else
	//	curTime = 0.0f;
}