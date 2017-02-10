#include <iostream>
#include "PlayerController.h"
#include "Camera.h"
#include "EventDispatcher.h"
#include "CapsuleCollider.h"
#include "GameObject.h"
#include "AnimatorController.h"
#include "BallController.h"
#include "Transform.h"
#include "InputManager.h"

using namespace std;

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

	//register as event handler
	EventDispatcher::GetSingleton()->RegisterHandler(this, GetGameObject()->GetName());

	//initialize to null
	otherPlayer = nullptr;
}

void PlayerController::Update(float dt)
{
	this->dt = dt;

	//transform->AddVelocity(float3(0, -9.8f * dt, 0));
}

void PlayerController::HandleEvent(Event* e)
{
	//filter throw events to find right one
	InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

	if (inputDownEvent)
	{
		//cout << inputDownEvent->GetInput()->GetMouseX() << " " << inputDownEvent->GetInput()->GetMouseY() << endl;

		if (inputDownEvent->IsServer())
		{
			string name;
			name = "Mage";
			name += to_string(inputDownEvent->GetID());

			if (GetGameObject()->GetName() == name)
			{
				input = inputDownEvent->GetInput();
				HandleInput();
			}
		}
	}
}

//misc//
void PlayerController::OnCollisionEnter(Collider* collider)
{
	CapsuleCollider* capsCollider = dynamic_cast<CapsuleCollider*>(collider);

	if (capsCollider)
	{
		if (capsCollider->GetGameObject()->GetName().find("Mage") != string::npos)
		{
			cout << "Collision enter" << endl;

			otherPlayer = capsCollider->GetGameObject();
		}
	}
}

void PlayerController::OnCollisionExit(Collider* collider)
{
	if (otherPlayer)
	{
		CapsuleCollider* capsCollider = dynamic_cast<CapsuleCollider*>(collider);

		if (capsCollider)
		{
			if (capsCollider->GetGameObject() == otherPlayer)
			{
				cout << "Collision exit" << endl;

				otherPlayer = nullptr;
			}
		}
	}
}

//private helper functions//
void PlayerController::HandleInput()
{
	if (input->GetKeyDown(VK_SPACE))
	{
		Jump();
	}

	if (input->GetKeyDown('F'))
	{
		cout << "F" << endl;
		Attack();
	}
}

void PlayerController::Jump()
{
	//if the player is touching the ground
	//{
	//	maybe translate too to make the jump better?
		//transform->AddVelocity({ 0, 20, 0 });
	//}

	//cout << "JUMP" << endl;
}

void PlayerController::Attack()
{
	//if colliding with another player
	if (otherPlayer)
	{
		const float attackForce = 1;

		XMFLOAT3 back = transform->GetForward();
		back = { -back.x, -back.y, -back.z };

		//otherPlayer->AddVelocity({ back.x * attackForce, back.y * attackForce, back.z * attackForce });

		//do animation
		AnimatorController* animator = otherPlayer->GetComponent<AnimatorController>();

		animator->SetTrigger("Stumble");

		cout << "Attack" << endl;

		//make them drop ball
		BallController* ball = otherPlayer->FindGameObject("GameBall")->GetComponent<BallController>();

		if (ball->GetHolder() == otherPlayer->GetTransform()->GetChild(1)->GetGameObject()) //if crosse == crosse
		{
			//ball->GetGameObject()->GetTransform()->SetPosition(ball->GetGameObject()->GetTransform()->GetParent()->GetPosition());
			ball->DropBall(otherPlayer);
		}
	}
}