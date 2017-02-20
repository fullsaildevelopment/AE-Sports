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

	timeSincePlayed = 0.0f;
}

void Movement::Update(float dt)
{
	this->dt = dt;

	//sound feedback
	if (isMoving && (timeSincePlayed == 0 || timeSincePlayed > 18.0f))
	{
		SoundEvent* soundEvent = new SoundEvent();
		soundEvent->Init(AK::EVENTS::PLAY_FOOTSTEPS__WALK____, GetGameObject()->FindIndexOfGameObject(GetGameObject()));
		EventDispatcher::GetSingleton()->DispatchTo(soundEvent, "Game");
		delete soundEvent;
		//cout << "play sand" << endl;
		timeSincePlayed = 0;
	}
	else if (!isMoving && timeSincePlayed)
	{
		SoundEvent* soundEvent = new SoundEvent();
		soundEvent->Init(AK::EVENTS::STOP_FOOTSTEPS__WALK____, GetGameObject()->FindIndexOfGameObject(GetGameObject()));
		EventDispatcher::GetSingleton()->DispatchTo(soundEvent, "Game");
		delete soundEvent;
		//cout << "stop sand" << endl;
	}

	if (isMoving)
	{
		timeSincePlayed += dt;
	}
	else
	{
		timeSincePlayed = 0;
	}

	//animation feedback
	if (isMoving)
	{
		AnimatorController* animator = GetGameObject()->GetComponent<AnimatorController>();

		if (animator)
		{
			int nextStateIndex = animator->GetNextStateIndex();

			if (animator->GetState(animator->GetCurrentStateIndex())->GetName() != "Run" && !animator->GetState(animator->GetNextStateIndex()))
			{
				animator->SetTrigger("Run");
				//cout << "Run" << endl;
			}
		}
	}
	else 
	{
		AnimatorController* animator = GetGameObject()->GetComponent<AnimatorController>();

		if (animator)
		{
			if (animator->GetState(animator->GetCurrentStateIndex())->GetName() != "Idle" && !animator->GetState(animator->GetNextStateIndex()))
			{
				animator->SetTrigger("Idle");
				//cout << "Idle" << endl;
			}
		}
	}

	isMoving = false;
}

void Movement::HandleEvent(Event* e)
{
	//filter events
	InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

	if (inputDownEvent)
	{
		if (inputDownEvent->IsServer())
		{
			string name;
			name = "Mage";
			name += to_string(inputDownEvent->GetID());

			if (GetGameObject()->GetName() == name)
			{
				HandleInput(inputDownEvent);
			}
		}
	}
}

//getters
bool Movement::IsMoving()
{
	return isMoving;
}

//private helper functions
void Movement::HandleInput(InputDownEvent* e)
{
	InputManager* input = e->GetInput();

	if (input->GetKey(forward))
	{
		XMFLOAT3 forward = transform->GetForward();
		forward = { -forward.x, -forward.y, -forward.z };
		transform->AddVelocity({ forward.x * moveSpeed, forward.y * moveSpeed,  forward.z * moveSpeed });
		//transform->Translate({ forward.x * moveSpeed * dt, forward.y * moveSpeed * dt,  forward.z * moveSpeed * dt });
		isMoving = true;
	}

	if (input->GetKey(back))
	{
		//transform->Translate({ 0.0f, 0.0f, -moveSpeed * dt });
		XMFLOAT3 forward = transform->GetForward();
		forward = { -forward.x, -forward.y, -forward.z };
		transform->AddVelocity({ forward.x * -moveSpeed, forward.y * -moveSpeed,  forward.z * -moveSpeed });
		//transform->Translate({ forward.x * -moveSpeed * dt, forward.y * -moveSpeed * dt,  forward.z * -moveSpeed * dt });
		isMoving = true;
	}

	if (input->GetKey(left))
	{
		XMFLOAT3 right = transform->GetRight();
		right = { -right.x, -right.y, -right.z };
		transform->AddVelocity({ right.x * -moveSpeed, right.y * -moveSpeed,  right.z * -moveSpeed });
		//transform->Translate({ right.x * -moveSpeed * dt, right.y * -moveSpeed * dt,  right.z * -moveSpeed * dt });
		isMoving = true;
	}

	if (input->GetKey(right))
	{
		XMFLOAT3 right = transform->GetRight();
		right = { -right.x, -right.y, -right.z };
		transform->AddVelocity({ right.x * moveSpeed, right.y * moveSpeed,  right.z * moveSpeed });
		//transform->Translate({ right.x * moveSpeed * dt, right.y * moveSpeed * dt,  right.z * moveSpeed * dt });
		//transform->AddVelocity({ moveSpeed, 0.0f, 0.0f });
		isMoving = true;
	}

	if (input->GetKey(up)) //up
	{
		XMFLOAT3 up = transform->GetUp();
		up = { -up.x, -up.y, -up.z };
		transform->Translate({ up.x * moveSpeed * dt, up.y * moveSpeed * dt,  up.z * moveSpeed * dt });
		//transform->AddVelocity({ 0.0f, moveSpeed, 0.0f });
		isMoving = true;
	}

	if (input->GetKey(down)) //down
	{
		XMFLOAT3 up = transform->GetUp();
		up = { -up.x, -up.y, -up.z };
		transform->Translate({ up.x * -moveSpeed * dt, up.y * -moveSpeed * dt,  up.z * -moveSpeed * dt });
		//transform->AddVelocity({ 0.0f, -moveSpeed, 0.0f });
		isMoving = true;
	}
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