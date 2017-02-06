#include "Movement.h"
#include "Transform.h"
#include "GameObject.h"
#include "InputDownEvent.h"
#include "EventDispatcher.h"
#include "SoundEngine.h"
#include "Game.h"

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

	cout << isMoving << endl;

	//sound feedback
	if (isMoving && (timeSincePlayed == 0 || timeSincePlayed > 18.0f))
	{
		SoundEngine::GetSingleton()->PostEvent(AK::EVENTS::PLAY_3D_FOOTSTEPSSAND, Game::GetClientID());
		timeSincePlayed = 0;
	}
	else if (!isMoving && timeSincePlayed)
	{
		SoundEngine::GetSingleton()->PostEvent(AK::EVENTS::STOP_3D_FOOTSTEPSSAND, Game::GetClientID());
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
		Renderer* renderer = GetGameObject()->GetComponent<Renderer>();
		if (renderer)
		{
			Blender* blender = renderer->GetBlender();
			if (blender)
			{
				//if (!blender->GetNextInterpolator()->HasAnimation())

				if (blender->GetCurInterpolator()->GetAnimation()->GetAnimationName() == "Idle")
				{
					Animation* nextAnimation = blender->GetNextInterpolator()->GetAnimation();
					bool runAnimation = false;

					if (nextAnimation)
					{
						if (nextAnimation->GetAnimationName() != "Run")
						{
							runAnimation = true;
							//GetGameObject()->GetComponent<Renderer>()->GetBlender()->SetNextAnimation("Run");
						}
					}
					else
					{
						runAnimation = true;
					}

					if (runAnimation)
					{
						GetGameObject()->GetComponent<Renderer>()->GetBlender()->SetNextAnimation("Run");
					}
				}
			}
		}
	}
	else
	{
		Renderer* renderer = GetGameObject()->GetComponent<Renderer>();
		if (renderer)
		{
			Blender* blender = renderer->GetBlender();
			if (blender)
			{
				if (blender->GetCurInterpolator()->GetAnimation()->GetAnimationName() != "Idle")
				{
					blender->SetNextAnimation("Idle");
				}
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
		//transform->AddVelocity({ 0.0f, 0.0f, moveSpeed});
		transform->Translate({ forward.x * moveSpeed * dt, forward.y * moveSpeed * dt,  forward.z * moveSpeed * dt });
		isMoving = true;
	}

	if (input->GetKey(back))
	{
		//transform->Translate({ 0.0f, 0.0f, -moveSpeed * dt });
		XMFLOAT3 forward = transform->GetForward();
		forward = { -forward.x, -forward.y, -forward.z };
		transform->Translate({ forward.x * -moveSpeed * dt, forward.y * -moveSpeed * dt,  forward.z * -moveSpeed * dt });
		//transform->AddVelocity({ 0.0f, 0.0f, -moveSpeed});
		isMoving = true;
	}

	if (input->GetKey(left))
	{
		XMFLOAT3 right = transform->GetRight();
		right = { -right.x, -right.y, -right.z };
		transform->Translate({ right.x * -moveSpeed * dt, right.y * -moveSpeed * dt,  right.z * -moveSpeed * dt });
		//transform->AddVelocity({ -moveSpeed, 0.0f, 0.0f });
		isMoving = true;
	}

	if (input->GetKey(right))
	{
		XMFLOAT3 right = transform->GetRight();
		right = { -right.x, -right.y, -right.z };
		transform->Translate({ right.x * moveSpeed * dt, right.y * moveSpeed * dt,  right.z * moveSpeed * dt });
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