#include <iostream>
#include "Crosse.h"
#include "EventDispatcher.h"
#include "SphereCollider.h"
#include "BallController.h"
#include "SoundEngine.h"
#include "Includes.h"
#include "GameObject.h"
#include "SoundEvent.h"
#include "Renderer.h"
#include "SoundEvent.h"
#include "CoughtEvent.h"
#include "Game.h"
#include "GamePadEvent.h"
#include "GamePad.h"
#include "Movement.h"

using namespace std;

Crosse::Crosse()
{
	magnetMultiplier = 1.0f;
	magnetSpeedMultiplier = 1.0f;
}

Crosse::~Crosse()
{

}

void Crosse::Init()
{
	//cache
	transform = GetGameObject()->GetTransform();
	minX = transform->GetPosition().x;
	minY = transform->GetPosition().y;
	ballTransform = GetGameObject()->FindGameObject("GameBall")->GetTransform();
	ballC = ballTransform->GetGameObject()->GetComponent<BallController>();
	//player = GetGameObject()->GetTransform()->GetParent()->GetParent()->GetGameObject();
	string mageName = "Mage";
	mageName += GetGameObject()->GetName()[6];
	player = GetGameObject()->FindGameObject(mageName);
	movement = player->GetComponent<Movement>();

	//register crosse event handler
	EventDispatcher::GetSingleton()->RegisterHandler(this, GetGameObject()->GetName());

	//catchAgainTimer = timeUntilCatchAgain;
}

void Crosse::Update(float _dt)
{
	//do magnet stuff
	if (!ballC->GetIsHeld())
	{
		XMFLOAT4X4* ball = ballTransform->GetWorldP();
		XMFLOAT4X4* net = transform->GetWorldP();
		float3 ball2net = float3(net->_41, net->_42, net->_43) - float3(ball->_41, ball->_42, ball->_43);
		float l = dot_product(ball2net, ball2net);

		if (l < 9.1f * magnetMultiplier)
		{
			float s = (2 * magnetSpeedMultiplier) / l;
			ballTransform->AddVelocity(ball2net.normalize() * s);
		}
	}
}

void Crosse::OnTriggerEnter(Collider* collider)
{
	if (collider->GetGameObject()->GetName() == "GameBall")
	{
		if (!ballC->GetIsHeld() && movement->CanMove()/* && catchAgainTimer >= timeUntilCatchAgain*/)
		{
			Catch();
		}
	}
}

//misc//
void Crosse::Throw()
{
	const float throwSpeed = 18.0f;

	if (ballC->GetCrosseHolder() == GetGameObject())
	{
		XMFLOAT4X4 ballworld = ballTransform->GetWorld();
		//detach ball
		XMFLOAT3 ballForward = transform->GetParent()->GetForward();
		ballTransform->SetPosition({ ballworld._41 + ballForward.x * 0.2f, ballworld._42 + ballForward.y * 0.2f, ballworld._43 + ballForward.z * 0.2f }); //set ball's position to real ball position

		ballC->Throw();
		SetColor(false);

		//update ball after set position
		ballTransform->GetWorld();

		ballTransform->AddVelocity(transform->GetParent()->GetParent()->GetVelocity() * 0.3f);
		ballTransform->AddVelocity({ ballForward.x * throwSpeed, ballForward.y * throwSpeed, ballForward.z * throwSpeed });

		//cout << ballTransform->GetVelocity().x << " " << ballTransform->GetVelocity().y << " " << ballTransform->GetVelocity().z << endl;

		// do animation on crosse
		//transform->RotateX(XMConvertToRadians(45));

		//play sound
		SoundEvent* soundEvent = new SoundEvent();
		soundEvent->Init(AK::EVENTS::PLAY_THROW, GetGameObject()->FindIndexOfGameObject(GetGameObject()));
		EventDispatcher::GetSingleton()->DispatchTo(soundEvent, "Game");
		delete soundEvent;

		//cout << "Throw" << endl;
	}
}

void Crosse::Catch()
{
	if (!ballC->GetHolder()) //there's a bug where on trigger enter calls catch a few times, so this prevents that
	{
		//increase player's stats
		PlayerController* playerController = player->GetComponent<PlayerController>();
		float3 goalPos;

		if (playerController->GetTeamID() == Game::PLAYER_TEAM::TEAM_A) //red player
		{
			goalPos = GetGameObject()->FindGameObject("RedGoal")->GetTransform()->GetPosition();
		}
		else //blue player
		{
			goalPos = GetGameObject()->FindGameObject("BlueGoal")->GetTransform()->GetPosition();
		}

		//dimensions of goal: 13 x 8

		//check the position to see if it was close to going to the goal
		float3 ballPos = ballTransform->GetWorldPosition();

		int direction = -1;

		if (playerController->GetTeamID() == Game::PLAYER_TEAM::TEAM_B) //blue player
		{
			direction = 1;
		}

		//cout << ballPos.x << " " << ballPos.y << " " << ballPos.z << "\t" << goalPos.x << " " << goalPos.y << " " << goalPos.z << endl;

		if ((ballPos.x <= goalPos.x + 8.0f || ballPos.x >= goalPos.x - 8.0f) && (ballPos.y >= goalPos.y - 4.0f) && ((ballPos.z >= goalPos.z - 8.0f && direction == 1) || (ballPos.z <= goalPos.z + 8.0f && direction == -1)))
		{
			//check the direction to make sure that it's not moving away from the goal
			if (dot_product(ballTransform->GetVelocity().normalize(), float3(0, 0, -1.0f * (float)direction)) <= 0)
			{
				player->GetComponent<PlayerController>()->AddSave();
			}
		}

		playerController->AddCatch();

		ballC->SetHolder(GetGameObject());
		SetColor(true);

		//play sound
		SoundEvent* soundEvent = new SoundEvent();
		soundEvent->Init(AK::EVENTS::PLAY_CATCH, GetGameObject()->FindIndexOfGameObject(GetGameObject()));
		EventDispatcher::GetSingleton()->DispatchTo(soundEvent, "Game");
		delete soundEvent;
	}
}

void Crosse::HandleEvent(Event* e)
{
	//filter throw events to find right one
	InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

	if (inputDownEvent)
	{
		//cout << inputDownEvent->GetInput()->GetMouseX() << " " << inputDownEvent->GetInput()->GetMouseY() << endl;

		//if (inputDownEvent->IsServer())
		{
			string name;
			name = "Crosse";
			name += to_string(inputDownEvent->GetID());

			if (GetGameObject()->GetName() == name)
			{
				HandleInput(inputDownEvent);
			}
		}
	}

	GamePadEvent* gamePadEvent = dynamic_cast<GamePadEvent*>(e);

	if (gamePadEvent)
	{
		string name;
		name = "Crosse";
		name += to_string(gamePadEvent->GetClientID());

		if (GetGameObject()->GetName() == name)
		{
			HandleGamePad(gamePadEvent);
		}

		return;
	}
}

//private helper functions//

void Crosse::HandleInput(InputDownEvent* e)
{
	if (!ResourceManager::GetSingleton()->IsPaused())
	{
		//temp cache
		input = e->GetInput();
		float3 position = transform->GetPosition();

		//cout << position.x << " " << position.y << " " << position.z << endl;

		//rotate the crosse
		//if (input->GetMouseX() && input->GetMouseY())
		//{
		//	if (input->GetMouseButton(0))
		//	{
		//		//move the crosse
		//		float xPos = (float)input->GetMouseX() - CLIENT_WIDTH / 2;
		//		float yPos = (float)input->GetMouseY() - CLIENT_HEIGHT / 2;

		//		float radians = 0;
		//		float yRadians = 0;
		//		bool doubleY = false;
		//		const int xWiggleRoom = 20; // to prevent it from rotating when cursor is in middle of screen
		//		float ratio = xPos / (CLIENT_WIDTH / 2);
		//		float yRatio = yPos / (CLIENT_HEIGHT / 2);

		//		if (xPos > xWiggleRoom && yPos > 0)
		//		{
		//			//radians = 135.0f / 180.0f * XM_PI;
		//			doubleY = true;
		//			//yPos *= 2.2f;
		//		}
		//		else if (xPos > xWiggleRoom && yPos < 0)
		//		{
		//			//radians = 45.0f / 180.0f * XM_PI;
		//		}
		//		else if (xPos < -xWiggleRoom && yPos < 0)
		//		{
		//			//radians = 45.0f / 180.0f * XM_PI;
		//			yRatio = -yRatio;
		//		}
		//		else if (xPos < -xWiggleRoom && yPos > 0)
		//		{
		//			//radians = 135.0f / 180.0f * XM_PI;
		//			doubleY = true;
		//			yRatio = -yRatio;
		//			//yPos *= 2.2f;
		//		}

		//		if (xPos > xWiggleRoom || xPos < -xWiggleRoom)
		//		{
		//			radians = -90.0f / 180.0f * XM_PI;
		//			yRadians = -45.0f / 180.0f * XM_PI;
		//		}

		//		if (doubleY)
		//		{
		//			yPos *= 2.2f; //added because crosse would go halfway down y
		//		}

		//		transform->SetPosition({ xPos * 0.001f * 1.8f, yPos * -0.001f + minY, transform->GetPosition().z }); // * 1.8 because * 2 is too much. And it was only travelling half
		//		transform->SetRotation({ transform->GetRotation().x, transform->GetRotation().y, (ratio * radians) + (yRatio * yRadians) });
		//		//transform->SetRotation({ transform->GetRotation().x, transform->GetRotation().y, (ratio * degrees) + (yRatio * 45.0f) });
		//	}
		//}

		if (input->GetMouseButtonDown(1))
		{
			Throw();
		}
	}
}

void Crosse::HandleGamePad(GamePadEvent* e)
{
	if (!ResourceManager::GetSingleton()->IsPaused())
	{
		//temp cache
		float3 position = transform->GetPosition();
		GamePad::State* padState = e->GetState();
		GamePad::ButtonStateTracker padTracker;

		padTracker.Update(*padState);

		//rotate the crosse
		//if (padState->IsLeftTriggerPressed())
		//{
		//	if (padState->thumbSticks.rightX || padState->thumbSticks.rightY)
		//	{
		//		//move the crosse

		//		float radians = 0;
		//		float yRadians = 0;
		//		bool doubleY = false;
		//		const int xWiggleRoom = 20; // to prevent it from rotating when cursor is in middle of screen
		//		float ratio = padState->thumbSticks.rightX;
		//		float yRatio = padState->thumbSticks.rightY;
		//		float xPos = CLIENT_WIDTH / 2 * ratio;
		//		float yPos = CLIENT_HEIGHT / 2 * yRatio;

		//		//cout << ratio << " " << yRatio << endl;

		//		if (xPos > 0 && yPos > 0) //top-right quadrant
		//		{
		//			//yRatio = -yRatio;
		//		}
		//		else if (xPos > 0 && yPos < 0) //bottom-right quadrant
		//		{
		//			//yRatio = -yRatio;
		//			doubleY = true;
		//			//ratio = -ratio;
		//			yRatio = -yRatio;
		//		}
		//		else if (xPos < 0 && yPos < 0) //bottom-left quadrant
		//		{
		//			//yRatio = -yRatio;
		//			doubleY = true;
		//			//yRatio = -yRatio;
		//			//ratio = -ratio;
		//		}
		//		else if (xPos < 0 && yPos > 0) //top-left quadrant
		//		{

		//		}

		//		//this way if the pos is inside the wiggle area, the radians will be zero and no rotation will happen
		//		if (xPos > 0 || xPos < 0)
		//		{
		//			radians = -90.0f / 180.0f * XM_PI;
		//			yRadians = -45.0f / 180.0f * XM_PI;
		//		}

		//		if (doubleY)
		//		{
		//			yPos *= 2.2f; //added because crosse would only go halfway down y in bottom quadrants
		//		}

		//		cout << xPos << " " << yPos << " " << (ratio * -90.0f) << " " << (yRatio * -45.0f) << " " << (ratio * -90.0f) + (yRatio * -45.0f) << endl;

		//		transform->SetPosition({ xPos * 0.001f * 1.8f, yPos * 0.001f + minY, transform->GetPosition().z }); // * 1.8 because * 2 is too much. And it was only travelling half
		//		transform->SetRotation({ transform->GetRotation().x, transform->GetRotation().y, (ratio * radians) + (yRatio * yRadians) });
		//		//transform->SetRotation({ transform->GetRotation().x, transform->GetRotation().y, (ratio * degrees) + (yRatio * 45.0f) });
		//	}
		//}

		if (padTracker.rightTrigger == GamePad::ButtonStateTracker::PRESSED)
		{
			Throw();
		}
	}
}

//getters//
float Crosse::GetMagnetMultiplier()
{
	return magnetMultiplier;
}

float Crosse::GetMagnetSpeedMultiplier()
{
	return magnetSpeedMultiplier;
}

//setters//
void Crosse::SetHolder(GameObject* object)
{
	if (ballC)
	{
		ballC->SetHolder(object);
	}
}

void Crosse::SetColor(bool b)
{
	if (GetGameObject())
	{
		if (b)
			GetGameObject()->GetComponent<Renderer>()->SetCatch(1.0f);
		else
			GetGameObject()->GetComponent<Renderer>()->SetCatch(0.0f);
		if (ResourceManager::GetSingleton()->IsServer())//Game::GetClientID() == 1)
		{
			CoughtEvent* ce = new CoughtEvent;
			ce->holding = b;
			ce->id = GetGameObject()->FindIndexOfGameObject(GetGameObject());
			EventDispatcher::GetSingleton()->DispatchTo(ce, "Game");
			delete ce;
		}
	}
}

void Crosse::SetMagnetMultiplier(float multiplier)
{
	magnetMultiplier = multiplier;
}

void Crosse::SetMagnetSpeedMultiplier(float multiplier)
{
	magnetSpeedMultiplier = multiplier;
}