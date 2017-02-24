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
#include "HexagonCollider.h"
#include "BoxCollider.h"
#include "State.h"

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
	floor = nullptr;
	justJumped = false;
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

			return;
		}
	}

	BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(collider);

	if (boxCollider)
	{
		if (boxCollider->GetGameObject()->GetName() == "MeterBox6")
		{
			if (justJumped)
			{
				justJumped = false;

				//do animation
				AnimatorController* animator = GetGameObject()->GetComponent<AnimatorController>();

				animator->SetTrigger("Land");

				cout << "Land" << endl;
			}

			floor = boxCollider->GetGameObject();

			cout << "box enter" << endl;

			return;
		}
	}

	HexagonCollider* hexCollider = dynamic_cast<HexagonCollider*>(collider);

	if (hexCollider)
	{
		if (justJumped)
		{
			justJumped = false;

			//do animation
			AnimatorController* animator = GetGameObject()->GetComponent<AnimatorController>();

			animator->SetTrigger("Land");
		}

		floor = hexCollider->GetGameObject();
		cout << "hex enter" << endl;

		return;
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

				return;
			}
		}
	}

	if (floor)
	{
		BoxCollider* boxCollider = dynamic_cast<BoxCollider*>(collider);

		if (boxCollider)
		{
			if (boxCollider->GetGameObject() == floor)
			{
				cout << "floor exit" << endl;

				floor = nullptr;
				return;
			}
		}

		HexagonCollider* hexCollider = dynamic_cast<HexagonCollider*>(collider);

		if (hexCollider)
		{
			if (hexCollider->GetGameObject() == floor)
			{
				cout << "hex exit" << endl;

				floor = nullptr;
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
		//cout << "F" << endl;
		Attack();
	}
}

void PlayerController::Jump()
{
	if (floor)
	{
		justJumped = true;
		transform->AddVelocity({ 0, 5.0f, 0 });
		cout << "JUMP" << endl;

		//do animation
		AnimatorController* animator = GetGameObject()->GetComponent<AnimatorController>();

		animator->SetTrigger("Jump");
		
		if (animator->GetState(animator->GetCurrentStateIndex())->GetName() == "Run")
		{
			animator->SetTrigger("Run");
		}
	}
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

		if (ball->GetCrosseHolder() == otherPlayer->GetTransform()->GetChild(0)->GetChild(0)->GetGameObject()) //if crosse == crosse
		{
			//ball->GetGameObject()->GetTransform()->SetPosition(ball->GetGameObject()->GetTransform()->GetParent()->GetPosition());
			ball->DropBall(otherPlayer);
		}
	}
}