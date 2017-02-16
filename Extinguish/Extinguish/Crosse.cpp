#include <iostream>
#include "Crosse.h"
#include "EventDispatcher.h"
#include "SphereCollider.h"
#include "BallController.h"
#include "SoundEngine.h"
#include "Includes.h"
#include "GameObject.h"
#include "SoundEvent.h"

using namespace std;

Crosse::Crosse()
{

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

	//register crosse event handler
	EventDispatcher::GetSingleton()->RegisterHandler(this, GetGameObject()->GetName());

	//catchAgainTimer = timeUntilCatchAgain;
}

void Crosse::Update(float dt)
{
	this->dt = dt;

	//catchAgainTimer += dt;
}

void Crosse::OnTriggerEnter(Collider* collider)
{
	if (collider->GetGameObject()->GetName() == "GameBall")
	{
		if (!ballC->GetIsHeld()/* && catchAgainTimer >= timeUntilCatchAgain*/)
		{
			ballC->SetHolder(GetGameObject());
			//catchAgainTimer = 0.0f;
		}
	}
}

//misc
void Crosse::Throw()
{
	const float throwSpeed = 25.0f;

	if (ballC->GetHolder() == GetGameObject())
	{
		XMFLOAT4X4 ballworld = ballTransform->GetWorld();
		//detach ball
		XMFLOAT3 ballForward = transform->GetParent()->GetForward();
		ballTransform->SetPosition({ ballworld._41 + ballForward.x * 0.2f, ballworld._42 + ballForward.y * 0.2f, ballworld._43 + ballForward.z * 0.2f }); //set ball's position to real ball position

		ballC->Throw();
		//update ball after set position
		ballTransform->GetWorld();

		ballTransform->AddVelocity({ ballForward.x * throwSpeed, ballForward.y * throwSpeed, ballForward.z * throwSpeed });

		//cout << ballTransform->GetVelocity().x << " " << ballTransform->GetVelocity().y << " " << ballTransform->GetVelocity().z << endl;

		// do animation on crosse
		//transform->RotateX(XMConvertToRadians(45));

		//play sound
		SoundEvent* soundEvent = new SoundEvent();
		soundEvent->Init(AK::EVENTS::PLAY_3D_SPEARBODY, GetGameObject()->FindIndexOfGameObject(GetGameObject()));
		EventDispatcher::GetSingleton()->DispatchTo(soundEvent, "Game");
		delete soundEvent;
		//SoundEngine::GetSingleton()->PostEvent(AK::EVENTS::PLAY_3D_SPEARBODY, 0);
		
		cout << "Throw" << endl;
	}
}

void Crosse::HandleEvent(Event* e)
{
	//filter throw events to find right one
	InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

	if (inputDownEvent)
	{
		//cout << inputDownEvent->GetInput()->GetMouseX() << " " << inputDownEvent->GetInput()->GetMouseY() << endl;

		if (inputDownEvent->IsServer())
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
}

//private helper functions//

void Crosse::HandleInput(InputDownEvent* e)
{
	//temp cache
	input = e->GetInput();
	float3 position = transform->GetPosition();

	//cout << position.x << " " << position.y << " " << position.z << endl;

	//rotate the crosse
	if (input->GetMouseX() && input->GetMouseY())
	{
		if (input->GetMouseButton(0))
		{
			//move the crosse
			float xPos = (float)input->GetMouseX() - CLIENT_WIDTH / 2;
			float yPos = (float)input->GetMouseY() - CLIENT_HEIGHT / 2;

			float radians = 0;
			float yRadians = 0;
			bool doubleY = false;
			const int xWiggleRoom = 20; // to prevent it from rotating when cursor is in middle of screen
			float ratio = xPos / (CLIENT_WIDTH / 2);
			float yRatio = yPos / (CLIENT_HEIGHT / 2);

			if (xPos > xWiggleRoom && yPos > 0)
			{
				//radians = 135.0f / 180.0f * XM_PI;
				doubleY = true;
				//yPos *= 2.2f;
			}
			else if (xPos > xWiggleRoom && yPos < 0)
			{
				//radians = 45.0f / 180.0f * XM_PI;
			}
			else if (xPos < -xWiggleRoom && yPos < 0)
			{
				//radians = 45.0f / 180.0f * XM_PI;
				yRatio = -yRatio;
			}
			else if (xPos < -xWiggleRoom && yPos > 0)
			{
				//radians = 135.0f / 180.0f * XM_PI;
				doubleY = true;
				yRatio = -yRatio;
				//yPos *= 2.2f;
			}

			if (xPos > xWiggleRoom || xPos < -xWiggleRoom)
			{
				radians = -90.0f / 180.0f * XM_PI;
				yRadians = -45.0f / 180.0f * XM_PI;
			}

			if (doubleY)
			{
				yPos *= 2.2f; //added because crosse would go halfway down y
			}

			transform->SetPosition({ xPos * 0.001f * 1.8f, yPos * -0.001f + minY, transform->GetPosition().z }); // * 1.8 because * 2 is too much. And it was only travelling half
			transform->SetRotation({ transform->GetRotation().x, transform->GetRotation().y, (ratio * radians) + (yRatio * yRadians) });
			//transform->SetRotation({ transform->GetRotation().x, transform->GetRotation().y, (ratio * degrees) + (yRatio * 45.0f) });
		}
	}

	if (input->GetMouseButtonDown(1))
	{
		Throw();
	}

	//if (input->GetKeyDown('T'))
	//{
	//	XMFLOAT3 up = transform->GetUp();
	//	transform->Translate({ up.x * dt, up.y * dt,  up.z * dt });
	//}
}

void Crosse::SetHolder(GameObject* object)
{
	BallController* ballController = GetGameObject()->FindGameObject("GameBall")->GetComponent<BallController>();

	if (ballController)
	{
		ballController->SetHolder(object);
	}
}