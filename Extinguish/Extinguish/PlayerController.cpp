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
#include <fstream>

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

void PlayerController::Update(float _dt)
{
	this->_dt = _dt;
	chargeTimer += _dt;
	//sprintAgainTimer += dt;

	HandleSprintAndCharge();

	Movement* movement = GetGameObject()->GetComponent<Movement>();

	PlayFootstepsSound();

	if (!movement->IsMoving() && footstepsPlayed)
	{
		StopFootstepsSound();
	}
}

void PlayerController::Shutdown()
{
	//save out player stats to a file
	ofstream bout;
	//name = "";
	string fileName = "../Resources/Stats/" + name + ".stats";

	bout.open(fileName, ios_base::binary);

	if (bout.is_open())
	{
		//write out name
		unsigned int nameSize = name.size();

		bout.write((const char*)&nameSize, sizeof(unsigned int));
		bout.write((const char*)&name[0], nameSize);

		//write out total goals
		bout.write((const char*)&totalGoals, sizeof(totalGoals));

		//write out total assists
		bout.write((const char*)&totalAssists, sizeof(totalAssists));

		//write out total saves
		bout.write((const char*)&totalSaves, sizeof(totalSaves));

		//write out total catches
		bout.write((const char*)&totalCatches, sizeof(totalCatches));
	}

	bout.close();
}

void PlayerController::HandleEvent(Event* e)
{
	//filter throw events to find right one
	InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

	if (inputDownEvent)
	{
		//cout << inputDownEvent->GetInput()->GetMouseX() << " " << inputDownEvent->GetInput()->GetMouseY() << endl;

		//if (inputDownEvent->IsServer())
		{
			string name;
			name = "Mage";
			name += to_string(inputDownEvent->GetID());

			if (GetGameObject()->GetName() == name)
			{
				//if (!Player::gamePadState.IsConnected())
				{
					playerID = inputDownEvent->GetID();
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

	if (collider->GetColliderType() == Collider::ColliderType::CTCapsule)
	{
		CapsuleCollider* capsCollider = (CapsuleCollider*)collider;
		if (capsCollider->GetGameObject()->GetName().find("Mage") != string::npos)
		{
			//cout << "Collision enter" << endl;

			otherPlayer = capsCollider->GetGameObject();

			Attack();

			return;
		}
	}

	if (collider->GetColliderType() == Collider::ColliderType::CTAABB)
	{
		BoxCollider* boxCollider = (BoxCollider*)collider;
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

	if (collider->GetColliderType() == Collider::ColliderType::CTHex)
	{
		HexagonCollider* hexCollider = (HexagonCollider*)collider;
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

void PlayerController::OnCollisionStay(Collider* collider)
{

	if (collider->GetColliderType() == Collider::ColliderType::CTAABB)
	{
		BoxCollider* boxCollider = (BoxCollider*)collider;
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
		return;
	}


	if (collider->GetColliderType() == Collider::ColliderType::CTHex)
	{
		HexagonCollider* hexCollider = (HexagonCollider*)collider;
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
		if (collider->GetColliderType() == Collider::ColliderType::CTCapsule)
		{
			CapsuleCollider* capsCollider = (CapsuleCollider*)collider;
			if (capsCollider->GetGameObject() == otherPlayer)
			{
				otherPlayer = nullptr;

				return;
			}
		}
	}

	if (floor)
	{
		if (collider->GetColliderType() == Collider::ColliderType::CTAABB)
		{
			BoxCollider* boxCollider = (BoxCollider*)collider;
			if (boxCollider->GetGameObject() == floor)
			{
				//cout << "floor exit" << endl;

				floor = nullptr;
				justJumped = false;
				return;
			}
		}

		if (collider->GetColliderType() == Collider::ColliderType::CTHex)
		{
			HexagonCollider* hexCollider = (HexagonCollider*)collider;
			if (hexCollider->GetGameObject() == floor)
			{
				floor = nullptr;
				justJumped = false;
			}
		}
	}
}

void PlayerController::AddGoal()
{
	++goals;
	++totalGoals;

	score += goalPoints;
	cout << "GOAL!" << endl;
}

void PlayerController::AddSave()
{
	++saves;
	++totalSaves;

	score += savePoints;

	cout << "SAVE!" << endl;
}

void PlayerController::AddAssist()
{
	++assists;
	++totalAssists;

	score += assistPoints;
	cout << "ASSIST!" << endl;
}

void PlayerController::AddCatch()
{
	++totalCatches;
}

void PlayerController::ReadInStats(std::string playerName)
{
	ifstream bin;
	string filePath = "../Resources/Stats/";
	filePath += playerName;
	filePath += ".stats";
	//WIN32_FIND_DATA fileData;
	//HANDLE filePathH = FindFirstFile(filePath.c_str(), &fileData);

	//if (filePathH != INVALID_HANDLE_VALUE)
	//{
	//	if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	//	{
	//		do
	//		{
	//string fileName = "../Resources/Stats/";
	//fileName += fileData.cFileName;

	bin.open(filePath, ios_base::binary);

	if (bin.is_open())
	{
		//read in name
		unsigned int nameSize;

		bin.read((char*)&nameSize, sizeof(unsigned int));

		name.resize(nameSize);
		bin.read((char*)&name[0], nameSize);

		//read in total goals
		bin.read((char*)&totalGoals, sizeof(totalGoals));

		//read in total assists
		bin.read((char*)&totalAssists, sizeof(totalAssists));

		//read intotal saves
		bin.read((char*)&totalSaves, sizeof(totalSaves));

		//read intotal catches
		bin.read((char*)&totalCatches, sizeof(totalCatches));
	}
	else //no file is there, so just take in name so respective file is made later
	{
		name = playerName;
	}
	bin.close();
	/*	} while (FindNextFile(filePathH, &fileData));
	}
	}*/
}

//actions//
void PlayerController::Jump()
{
	if (floor && !justJumped)
	{
		justJumped = true;
		transform->AddVelocity({ 0, 4.0f, 0 });
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

		//cout << "Sprint" << endl;
	}
	else if ((input->GetKeyUp(16) || input->GetKeyUp('W')) && isSprinting)
	{
		isSprinting = false;
		isCharging = false;

		Physics* physics = GetGameObject()->GetComponent<Physics>();
		physics->SetMaxSpeed(originalMaxSpeed);
		//physics->SetHasMaxSpeed(true);

		//cout << "Stop Sprint" << endl;

		//revert back to walk footsteps
		SetFootstepsSound(0);
	}
}

void PlayerController::HandleGamePad(GamePadEvent* gamePadEvent)
{
	GamePad::State* padState = gamePadEvent->GetState();
	GamePad::ButtonStateTracker tracker;

	tracker.Update(*padState);

	if (tracker.a == GamePad::ButtonStateTracker::PRESSED)
	{
		Jump();
	}

	//this line will only happen once
	if (padState->IsLeftStickPressed() && padState->thumbSticks.leftY && !isSprinting && canSprint) //16 == Left Shift
	{
		Sprint();

		//cout << "Sprint" << endl;
	}
	else if (!padState->thumbSticks.leftY && isSprinting)
	{
		isSprinting = false;
		isCharging = false;

		Physics* physics = GetGameObject()->GetComponent<Physics>();
		physics->SetMaxSpeed(originalMaxSpeed);
		//physics->SetHasMaxSpeed(true);

		//cout << "Stop Sprint" << endl;

		//revert back to walk footsteps
		SetFootstepsSound(0);
	}
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

		if (!ResourceManager::GetSingleton()->IsMultiplayer() || playerID == Game::GetClientID())
		{
			if (meterBar->isEmpty()) // empty, no more sprint
			{
				canSprint = false;
				isCharging = false;
				SetFootstepsSound(0);
			}
		}
		else
		{
			if (Game::server.getEmpty(playerID - 1)) 
			{
				canSprint = false;
				isCharging = false;
				SetFootstepsSound(0);
			}
		}

		//set velocity to respective velocity every frame
		transform->AddVelocity(transform->GetForwardf3() * -speedMultiplier * 10.0f);
	}
	else if (!canSprint) //if you can't sprint, look for reasons to sprint
	{
		if (!ResourceManager::GetSingleton()->IsMultiplayer() || playerID == Game::GetClientID())
		{
			if ((!meterBar->isDraining() && !meterBar->getActive()) || !meterBar->isEmpty())
			{
				canSprint = true;
			}
		}
		else
		{
			if (!Game::server.getEmpty(playerID - 1) || (!Game::server.getMeterDrain(playerID - 1) && !Game::server.getMeterActive(playerID - 1)))
			{
				canSprint = true;
			}
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
		//cout << "play walk" << endl;

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

	//cout << "stop walk" << endl;
	footstepsPlayed = false;
}

void PlayerController::SetFootstepsSound(int sound)
{
	//stop playing previous footstepsSound
	StopFootstepsSound();

	//set it
	footstepsSound = sound;
}

void PlayerController::ResetPlayerGameStats()
{
	score = 0;
	goals = 0;
	assists = 0;
	saves = 0;
}

//getters//
std::string PlayerController::GetName()
{
	return name;
}

unsigned int PlayerController::GetTotalGoals()
{
	return totalGoals;
}

unsigned int PlayerController::GetTotalSaves()
{
	return totalSaves;
}


unsigned int PlayerController::GetTotalAssists()
{
	return totalAssists;
}


unsigned int PlayerController::GetScore()
{
	return score;
}


unsigned int PlayerController::GetGoals()
{
	return goals;
}


unsigned int PlayerController::GetSaves()
{
	return saves;
}


unsigned int PlayerController::GetAssists()
{
	return assists;
}

unsigned int PlayerController::GetTeamID()
{
	return teamID;
}

//setters//
void PlayerController::SetName(std::string name)
{
	this->name = name;
}

void PlayerController::SetTotalGoals(unsigned int ttlGoals)
{
	totalGoals = ttlGoals;
}

void PlayerController::SetTotalSaves(unsigned int ttlSaves)
{
	totalSaves = ttlSaves;
}

void PlayerController::SetTotalAssists(unsigned int ttlAssists)
{
	totalAssists = ttlAssists;
}

void PlayerController::SetTeamID(unsigned int id)
{
	teamID = id;
}