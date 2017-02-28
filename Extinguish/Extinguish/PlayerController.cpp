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
#include "MeterBar.h"
#include "SoundEngine.h"
#include "SoundEvent.h"
#include "Movement.h"

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
	isSprinting = false;
	footstepsSound = 0;
	footstepsPlayed = false;

	//other initialization
	canSprint = true;
}

void PlayerController::Update(float dt)
{
	this->dt = dt;
	chargeTimer += dt;
	//sprintAgainTimer += dt;

	HandleSprintAndCharge();

	Movement* movement = GetGameObject()->GetComponent<Movement>();

	PlayFootstepsSound();

	if (!movement->IsMoving() && footstepsPlayed)
	{
		StopFootstepsSound();
	}
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

			Attack();

			//	const float attackForce = 1;

			//	XMFLOAT3 back = transform->GetForward();
			//	back = { -back.x, -back.y, -back.z };

			//	//otherPlayer->AddVelocity({ back.x * attackForce, back.y * attackForce, back.z * attackForce });

			//	//do animation
				//AnimatorController* animator = otherPlayer->GetComponent<AnimatorController>();

				//animator->SetTrigger("Stumble");

				//cout << "Attack" << endl;

				////make them drop ball
				//BallController* ball = otherPlayer->FindGameObject("GameBall")->GetComponent<BallController>();

				//if (ball->GetCrosseHolder() == otherPlayer->GetTransform()->GetChild(0)->GetChild(0)->GetGameObject()) //if crosse == crosse
				//{
				//	//ball->GetGameObject()->GetTransform()->SetPosition(ball->GetGameObject()->GetTransform()->GetParent()->GetPosition());
				//	ball->DropBall(otherPlayer);
				//}

				//isAttacking = false;

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
	//if (otherPlayer)
	//{
	//	const float attackForce = 1;

	//	XMFLOAT3 back = transform->GetForward();
	//	back = { -back.x, -back.y, -back.z };

	//	//otherPlayer->AddVelocity({ back.x * attackForce, back.y * attackForce, back.z * attackForce });


	if (isCharging)
	{
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
	//}

	//isAttacking = true;

	//transform->SetVelocity(transform->GetForwardf3() * -chargeSpeed);
}

void PlayerController::Sprint()
{
	isSprinting = true;
	//chargeTimer = 0.0f;

	Physics* physics = GetGameObject()->GetComponent<Physics>();
	//physics->SetHasMaxSpeed(false);
	originalMaxSpeed = physics->GetMaxSpeed();
	physics->SetMaxSpeed(originalMaxSpeed * sprintMultiplier);

	//play sprint footsteps sound
	SetFootstepsSound(1);
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

	//this line will only happen once
	if (input->GetKey(16) && input->GetKey('W') && !isSprinting && canSprint) //16 == Left Shift
	{
		Sprint();

		//Play sound


		cout << "Sprint" << endl;
	}
	else if ((input->GetKeyUp(16) || input->GetKeyUp('W')) && isSprinting)
	{
		isSprinting = false;
		isCharging = false;

		Physics* physics = GetGameObject()->GetComponent<Physics>();
		physics->SetMaxSpeed(originalMaxSpeed);
		//physics->SetHasMaxSpeed(true);

		cout << "Stop Sprint" << endl;

		//revert back to walk footsteps
		SetFootstepsSound(0);
	}
}

void PlayerController::HandleSprintAndCharge()
{
	float multiplier;
	MeterBar* meterBar = GetGameObject()->FindUIObject("meterBar")->GetComponent<MeterBar>();

	if (isSprinting && canSprint)
	{
		if (isCharging) // wat
		{
			multiplier = chargeMultiplier;

			//TODO: do some visual effect

		}
		else if (chargeTimer >= timeTilCharge && !isCharging) // wat
		{
			multiplier = chargeMultiplier;
			//isSprinting = false;
			isCharging = true;

			cout << "Charge time" << endl;

			//TODO: play charge sound

			//set footsteps to charge
			SetFootstepsSound(2);

			//TODO: do some visual effect
		}
		else
		{
			multiplier = sprintMultiplier;
		}

	//	float percentage = meterBar->GetPercentage() - sprintCost * dt;

		//if small enough, then make it 0
		//if (percentage <= 0.001f)
		if (!meterBar->isDraining() && meterBar->getActive()) // recharging
		{
			//sprintAgainTimer = 0.0f;
	//		percentage = 0.0f;
			canSprint = false;
			isCharging = false;
		}
		else if (!meterBar->isDraining() && !meterBar->getActive()) // fully charged
		{
			canSprint = true;
		}


		//set velocity to respective velocity every frame
		transform->SetVelocity(transform->GetForwardf3() * -multiplier * 100.0f);
	}

	//this was going to be the code for if we had the sprint bar regenerate

	//else if (!canSprint)
	//{
	//	if (sprintAgainTimer >= timeTilSprint)
	//	{
	//		canSprint = true;

	//		//
	//	}
	//}
}

void PlayerController::PlayFootstepsSound()
{
	AkUniqueID playID;

	if (GetGameObject()->FindIndexOfGameObject(GetGameObject()) == 2)
	{
		cout << footstepsSound << endl;
	}

	switch (footstepsSound)
	{
	case 0:
		playID = AK::EVENTS::PLAY_FOOTSTEPS__WALK____;

		break;
	case 1:
		playID = AK::EVENTS::PLAY_FOOTSTEPS__SPRINT_;

		break;
	case 2:
		playID = AK::EVENTS::PLAY_FOOTSTEPS__CHARGE_;

		break;
	}

	Movement* movement = GetGameObject()->GetComponent<Movement>();

	if (movement->IsMoving() && !footstepsPlayed)
	{
		SoundEvent* soundEvent = new SoundEvent();
		soundEvent->Init(playID, GetGameObject()->FindIndexOfGameObject(GetGameObject()));
		EventDispatcher::GetSingleton()->DispatchTo(soundEvent, "Game");
		delete soundEvent;
		cout << "play walk" << endl;

		footstepsPlayed = true;
	}
}

void PlayerController::StopFootstepsSound()
{
	AkUniqueID stopID;

	switch (footstepsSound)
	{
	case 0:
		stopID = AK::EVENTS::STOP_FOOTSTEPS__WALK____;

		break;
	case 1:
		stopID = AK::EVENTS::STOP_FOOTSTEPS__SPRINT_;

		break;
	case 2:
		stopID = AK::EVENTS::STOP_FOOTSTEPS__CHARGE_;

		break;
	}

	Movement* movement = GetGameObject()->GetComponent<Movement>();

	SoundEvent* soundEvent = new SoundEvent();
	soundEvent->Init(stopID, GetGameObject()->FindIndexOfGameObject(GetGameObject()));
	EventDispatcher::GetSingleton()->DispatchTo(soundEvent, "Game");
	delete soundEvent;

	cout << "stop walk" << endl;
	footstepsPlayed = false;
}

void PlayerController::SetFootstepsSound(int sound)
{
	//stop playing previous footstepsSound
	StopFootstepsSound();

	//set it
	footstepsSound = sound;
}