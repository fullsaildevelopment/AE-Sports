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
#include "GamePad.h"
#include "Game.h"
#include "GamePadEvent.h"

using namespace std;

//charge is the ability to attack a player (and it makes you go faster)
//after *blank* seconds of sprinting, you automatically charge

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
				//if (!Player::gamePadState.IsConnected())
				{
					input = inputDownEvent->GetInput();
					HandleInput();
				}
			}
		}

		return;
	}

	GamePadEvent* gamePadEvent = dynamic_cast<GamePadEvent*>(e);

	if (gamePadEvent)
	{
		string name;
		name = "Mage";
		name += to_string(gamePadEvent->GetClientID());

		if (GetGameObject()->GetName() == name)
		{
			HandleGamePad(gamePadEvent);
		}

		return;
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
			//cout << "Collision enter" << endl;

			otherPlayer = capsCollider->GetGameObject();

			Attack();

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

				//cout << "Land" << endl;
			}

			floor = boxCollider->GetGameObject();

			//cout << "box enter" << endl;

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
		//cout << "hex enter" << endl;

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
				//cout << "floor exit" << endl;

				floor = nullptr;
				justJumped = false;
				return;
			}
		}

		HexagonCollider* hexCollider = dynamic_cast<HexagonCollider*>(collider);

		if (hexCollider)
		{
			if (hexCollider->GetGameObject() == floor)
			{
				floor = nullptr;
				justJumped = false;
			}
		}
	}
}

void PlayerController::Jump()
{
	if (floor && !justJumped)
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
}

void PlayerController::Sprint()
{
	isSprinting = true;
	chargeTimer = 0.0f;

	Physics* physics = GetGameObject()->GetComponent<Physics>();
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

	//this line will only happen once
	if (input->GetKey(16) && input->GetKey('W') && !isSprinting && canSprint) //16 == Left Shift
	{
		Sprint();

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

void PlayerController::HandleGamePad(GamePadEvent* gamePadEvent)
{

	GamePad::ButtonStateTracker tracker;

	tracker.Update(*gamePadEvent->GetState());

	if (tracker.a == GamePad::ButtonStateTracker::PRESSED)
	{
		Jump();
	}

	//this line will only happen once
	//if ( && !isSprinting && canSprint) //16 == Left Shift
	//{
	//	Sprint();

	//	//Play sound


	//	cout << "Sprint" << endl;
	//}
	//else if (input->GetKeyUp(16) && isSprinting)
	//{
	//	isSprinting = false;
	//	isCharging = false;

	//	Physics* physics = GetGameObject()->GetComponent<Physics>();
	//	physics->SetMaxSpeed(originalMaxSpeed);
	//	//physics->SetHasMaxSpeed(true);

	//	cout << "Stop Sprint" << endl;

	//	//revert back to walk footsteps
	//	SetFootstepsSound(0);
	//}
}

//sprinting isn't necessarily charging but charging is sprinting
void PlayerController::HandleSprintAndCharge()
{
	float speedMultiplier;
	MeterBar* meterBar = GetGameObject()->FindUIObject("sprintBar")->GetComponent<MeterBar>();

	if (isSprinting && canSprint)
	{
		if (isCharging) //if currently charging, just change speed multiplier
		{
			speedMultiplier = chargeMultiplier;

			//TODO: do some visual effect

		}
		else if (chargeTimer >= timeTilCharge && !isCharging)  //if not charging but you should be charging, charge
		{
			speedMultiplier = chargeMultiplier;
			isCharging = true;

			cout << "Charge time" << endl;

			//TODO: play charge sound

			//set footsteps to charge
			SetFootstepsSound(2);

			//TODO: do some visual effect
		}
		else //if not charging then sprint
		{
			speedMultiplier = sprintMultiplier;
		}

		if (meterBar->isEmpty()) // empty, no more sprint
		{
			canSprint = false;
			isCharging = false;
			SetFootstepsSound(0);
		}

		//set velocity to respective velocity every frame
		transform->AddVelocity(transform->GetForwardf3() * -speedMultiplier * 10.0f);
	}
	else if (!canSprint) //if you can't sprint, look for reasons to sprint
	{
		if ((!meterBar->isDraining() && !meterBar->getActive()) || !meterBar->isEmpty())
		{
			canSprint = true;
		}
	}
}

void PlayerController::PlayFootstepsSound()
{
	AkUniqueID playID;

	if (GetGameObject()->FindIndexOfGameObject(GetGameObject()) == 2)
	{
		//cout << footstepsSound << endl;
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