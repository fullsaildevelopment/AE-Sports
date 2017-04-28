#include "Movement.h"
#include "Transform.h"
#include "GameObject.h"
#include "InputDownEvent.h"
#include "EventDispatcher.h"
#include "Game.h"
#include "AnimatorController.h"
#include "State.h"
#include "Trigger.h"
#include "SoundEngine.h"
#include "SoundEvent.h"
#include "GamePad.h"
#include "GamePadEvent.h"
#include "Physics.h"
#include "CanPlayEvent.h"
#include "AI.h"

//namespace Move
//{
//	DirectX::GamePad::State gamePadState;
//};

void Movement::Init(float moveVelocity, float rotateVelocity)
{
	//cache
	transform = GetGameObject()->GetTransform();

	moveSpeed = moveVelocity;
	rotateSpeed = rotateVelocity;

	//set keys to default values
	//SetKeys('I', 'K', 'J', 'L', 'U', 'P');
	SetKeys('W', 'S', 'A', 'D', 'Q', 'E');

	//register movement event handler
	EventDispatcher::GetSingleton()->RegisterHandler(this, GetGameObject()->GetName());

	canMove = false;

	//timeSincePlayed = 0.0f;
	//footstepsPlayed = false;
}

void Movement::Update(float _dt)
{
	this->_dt = _dt;
	//animation feedback
	//string handsName = "TitanHands";

	//AnimatorController* animator = GetGameObject()->FindGameObject(
	//if (isMoving)
	if (isMoving)
	{
		AnimatorController* animator = GetGameObject()->GetComponent<AnimatorController>();

		if (animator)
		{
			int nextStateIndex = animator->GetNextStateIndex();

			if (animator->GetState(animator->GetCurrentStateIndex())->GetName() != "Run" && !animator->GetState(animator->GetNextStateIndex()) && GetGameObject()->GetComponent<PlayerController>()->IsSprinting())
			{
				animator->SetTrigger("Run");
				//cout << "Run" << endl;
			}
			else if (animator->GetState(animator->GetCurrentStateIndex())->GetName() != "Jog Forward" && !animator->GetState(animator->GetNextStateIndex()))
			{
				animator->SetTrigger("Jog Forward");
			}
		}
	}
	else 
	{
		AnimatorController* animator = GetGameObject()->GetComponent<AnimatorController>();
		AI* ai = GetGameObject()->GetComponent<AI>();

		if (animator && !ai)
		{
			if (animator->GetState(animator->GetCurrentStateIndex())->GetName() != "Idle" && !animator->GetState(animator->GetNextStateIndex()))
			{
				animator->SetTrigger("Idle");
				//cout << "Idle" << endl;
			}
		}
	}
}

void Movement::HandleEvent(Event* e)
{
	if (!ResourceManager::GetSingleton()->IsPaused()) {
		//filter events
		InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

		if (inputDownEvent)
		{
			//if (inputDownEvent->IsServer())
			{
				string name;
				name = "Mage";
				name += to_string(inputDownEvent->GetID());

				if (GetGameObject()->GetName() == name)
				{
					HandleInput(inputDownEvent);
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

	CanPlayEvent* playEvent = dynamic_cast<CanPlayEvent*>(e);

	if (playEvent)
	{
		canMove = playEvent->CanPlay();

		return;
	}
}

//getters//
bool Movement::IsMoving()
{
	return isMoving;
}

bool Movement::CanMove()
{
	return canMove;
}

float Movement::GetMoveSpeed()
{
	return moveSpeed;
}


//setters//
void Movement::SetKeys(char forward, char back, char left, char right, char up, char down)
{
	this->forward = forward;
	this->back = back;
	this->left = left;
	this->right = right;
	this->up = up;
	this->down = down;
}

void Movement::SetCanMove(bool toggle)
{
	canMove = toggle;
}

void Movement::SetMoveSpeed(float speed)
{
	moveSpeed = speed;
}

//private helper functions
void Movement::HandleInput(InputDownEvent* e)
{
	InputManager* input = e->GetInput();
	isMoving = false;
	Physics* phys = GetGameObject()->GetComponent<Physics>();
	bool af = true;

	AnimatorController* animator = GetGameObject()->GetComponent<AnimatorController>();

	if (canMove)
	{
		if (input->GetKey(forward))
		{
			XMFLOAT3 forward = transform->GetForward();
			//forward = { forward.x, forward.y, forward.z };

			transform->AddVelocity({ forward.x * moveSpeed * _dt, forward.y * moveSpeed * _dt,  forward.z * moveSpeed * _dt });

			//transform->Translate({ forward.x * moveSpeed * dt, forward.y * moveSpeed * dt,  forward.z * moveSpeed * dt });
			isMoving = true;
			af = false;

			//if (animator->GetState(animator->GetCurrentStateIndex())->GetName() != "Jog Forward" && !animator->GetState(animator->GetNextStateIndex()))
			//{
			//	animator->SetTrigger("Jog Forward");
			//}
		}

		if (input->GetKey(back))
		{
			//transform->Translate({ 0.0f, 0.0f, -moveSpeed * dt });
			XMFLOAT3 forward = transform->GetForward();
			forward = { -forward.x, -forward.y, -forward.z };

			transform->AddVelocity({ forward.x * moveSpeed* _dt, forward.y * moveSpeed* _dt,  forward.z * moveSpeed * _dt });
			//transform->Translate({ forward.x * -moveSpeed * dt, forward.y * -moveSpeed * dt,  forward.z * -moveSpeed * dt });
			isMoving = true;
			af = false;

			//if (animator->GetState(animator->GetCurrentStateIndex())->GetName() != "Jog Backward" && !animator->GetState(animator->GetNextStateIndex()))
			//{
			//	animator->SetTrigger("Jog Backward");
			//}
		}

		if (input->GetKey(left))
		{
			XMFLOAT3 right = transform->GetRight();
			XMFLOAT3 left = { -right.x, -right.y, -right.z };

			transform->AddVelocity({ left.x * moveSpeed* _dt, left.y * moveSpeed* _dt,  left.z * moveSpeed * _dt });
			//transform->Translate({ right.x * -moveSpeed * dt, right.y * -moveSpeed * dt,  right.z * -moveSpeed * dt });
			isMoving = true;
			af = false;

			//if (animator->GetState(animator->GetCurrentStateIndex())->GetName() != "Jog Left" && !animator->GetState(animator->GetNextStateIndex()))
			//{
			//	animator->SetTrigger("Jog Left");
			//}
		}

		if (input->GetKey(right))
		{
			XMFLOAT3 right = transform->GetRight();
			//right = { -right.x, -right.y, -right.z };

			transform->AddVelocity({ right.x * moveSpeed* _dt, right.y * moveSpeed* _dt,  right.z * moveSpeed * _dt });
			//transform->Translate({ right.x * moveSpeed * dt, right.y * moveSpeed * dt,  right.z * moveSpeed * dt });
			//transform->AddVelocity({ moveSpeed, 0.0f, 0.0f });
			isMoving = true;
			af = false;

			//if (animator->GetState(animator->GetCurrentStateIndex())->GetName() != "Jog Right" && !animator->GetState(animator->GetNextStateIndex()))
			//{
			//	animator->SetTrigger("Jog Right");
			//}
		}
	}

	phys->SetApplyFriction(af);
	//if (input->GetKey(up)) //up
	//{
	//	XMFLOAT3 up = transform->GetUp();
	//	up = { -up.x, -up.y, -up.z };
	//	transform->Translate({ up.x * moveSpeed * dt, up.y * moveSpeed * dt,  up.z * moveSpeed * dt });
	//	//transform->AddVelocity({ 0.0f, moveSpeed, 0.0f });
	//	isMoving = true;
	//}

	//if (input->GetKey(down)) //down
	//{
	//	XMFLOAT3 up = transform->GetUp();
	//	up = { -up.x, -up.y, -up.z };
	//	transform->Translate({ up.x * -moveSpeed * dt, up.y * -moveSpeed * dt,  up.z * -moveSpeed * dt });
	//	//transform->AddVelocity({ 0.0f, -moveSpeed, 0.0f });
	//	isMoving = true;
	//}
}

void Movement::HandleGamePad(GamePadEvent* gamePadEvent)
{
	isMoving = false;

	GamePad::State* state = gamePadEvent->GetState();

	//GamePad::ButtonStateTracker tracker;

	//tracker.Update(gamePadState);

	float leftY = state->thumbSticks.leftY;

	if (leftY > 0.0f)
	{
		XMFLOAT3 forward = transform->GetForward();
		forward = { -forward.x, -forward.y, -forward.z };
		transform->AddVelocity({ forward.x * moveSpeed * leftY, forward.y * moveSpeed * leftY,  forward.z * moveSpeed * leftY });
		//transform->Translate({ forward.x * moveSpeed * dt, forward.y * moveSpeed * dt,  forward.z * moveSpeed * dt });
		isMoving = true;
	}

	if (leftY < 0.0f)
	{
		//transform->Translate({ 0.0f, 0.0f, -moveSpeed * dt });
		XMFLOAT3 forward = transform->GetForward();
		forward = { -forward.x, -forward.y, -forward.z };
		transform->AddVelocity({ forward.x * moveSpeed * leftY, forward.y * moveSpeed * leftY,  forward.z * moveSpeed * leftY });
		//transform->Translate({ forward.x * -moveSpeed * dt, forward.y * -moveSpeed * dt,  forward.z * -moveSpeed * dt });
		isMoving = true;
	}

	float leftX = state->thumbSticks.leftX;


	if (leftX < 0.0f)
	{
		XMFLOAT3 right = transform->GetRight();
		right = { -right.x, -right.y, -right.z };
		transform->AddVelocity({ right.x * moveSpeed * leftX, right.y * moveSpeed * leftX,  right.z * moveSpeed * leftX });
		//transform->Translate({ right.x * -moveSpeed * dt, right.y * -moveSpeed * dt,  right.z * -moveSpeed * dt });
		isMoving = true;
	}

	if (leftX > 0.0f)
	{
		XMFLOAT3 right = transform->GetRight();
		right = { -right.x, -right.y, -right.z };
		transform->AddVelocity({ right.x * moveSpeed * leftX, right.y * moveSpeed * leftX,  right.z * moveSpeed * leftX });
		//transform->Translate({ right.x * moveSpeed * dt, right.y * moveSpeed * dt,  right.z * moveSpeed * dt });
		//transform->AddVelocity({ moveSpeed, 0.0f, 0.0f });
		isMoving = true;
	}
}