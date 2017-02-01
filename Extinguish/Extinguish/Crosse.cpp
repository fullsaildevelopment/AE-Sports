#include <iostream>
#include "Crosse.h"
#include "EventDispatcher.h"
#include "SphereCollider.h"
#include "BallController.h"
#include "SoundEngine.h"
#include "Includes.h"

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

	//register crosse event handler
	EventDispatcher::GetSingleton()->RegisterHandler(this);

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
		BallController* ballController = collider->GetGameObject()->GetComponent<BallController>();
		if (!ballController->GetIsHeld()/* && catchAgainTimer >= timeUntilCatchAgain*/)
		{
			ballController->SetHolder(GetGameObject());
			//catchAgainTimer = 0.0f;
		}
	}
}

//misc
void Crosse::Throw()
{
	const float throwSpeed = 1.0f;

	BallController* ball = ballTransform->GetGameObject()->GetComponent<BallController>();
	if (ball->GetHolder() == GetGameObject())
	{
		//detach ball
		ballTransform->SetPosition({ ballTransform->GetWorld()._41, ballTransform->GetWorld()._42, ballTransform->GetWorld()._43 });
		ballTransform->SetRotation({ transform->GetParent()->GetRotation().x, transform->GetParent()->GetRotation().y, transform->GetParent()->GetRotation().z });
		ballTransform->SetParent(nullptr);
		transform->RemoveChild(ballTransform);
		ball->Throw();

		//update ball after set position
		ballTransform->GetWorld();

		//add force to ball
		XMFLOAT3 ballForward = ballTransform->GetForward();
		ballForward = { -ballForward.x, -ballForward.y, -ballForward.z };
		//ballForward = { 0, 0, 1 };
		ballTransform->AddVelocity({ ballForward.x * throwSpeed, ballForward.y * throwSpeed, ballForward.z * throwSpeed });

		//cout << ballTransform->GetVelocity().x << " " << ballTransform->GetVelocity().y << " " << ballTransform->GetVelocity().z << endl;

		// do animation on crosse
		//transform->RotateX(XMConvertToRadians(45));

		//play sound
		SoundEngine::GetSingleton()->PlaySpearSound();
		
		//cout << "Throw" << endl;
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
			const int xWiggleRoom = 20;
			float ratio = xPos / (CLIENT_WIDTH / 2);
			float yRatio = yPos / (CLIENT_HEIGHT / 2);

			if (xPos > xWiggleRoom && yPos > 0)
			{
				radians = 135.0f / 180.0f * XM_PI;
				doubleY = true;
				//yPos *= 2.2f;
			}
			else if (xPos > xWiggleRoom && yPos < 0)
			{
				radians = 45.0f / 180.0f * XM_PI;
			}
			else if (xPos < -xWiggleRoom && yPos < 0)
			{
				radians = 45.0f / 180.0f * XM_PI;
				yRatio = -yRatio;
			}
			else if (xPos < -xWiggleRoom && yPos > 0)
			{
				radians = 135.0f / 180.0f * XM_PI;
				doubleY = true;
				yRatio = -yRatio;
				//yPos *= 2.2f;
			}

			if (xPos > xWiggleRoom || xPos < -xWiggleRoom)
			{
				radians = 90.0f / 180.0f * XM_PI;
				yRadians = 45.0f / 180.0f * XM_PI;
			}

			//cout << xPos << endl;

			//cout << xPos << " " << yPos << endl;
			//cout << xPos * -0.001f << " " << yPos * -0.001f << endl;

			//rotate crosse

			//float degreesRatio = ratio * yRatio / 135.0f;

			//cout << ratio << " " << yRatio << endl;
			//cout << degreesRatio << endl;
			//cout << radians << " " << yRadians << endl;

			if (doubleY)
			{
				yPos *= 2.2f;
			}

			//cout << doubleY;
			//cout << radians;

			transform->SetPosition({ xPos * -0.001f * 1.8f, yPos * -0.001f + minY, transform->GetPosition().z }); // * 1.8 because * 2 is too much. And it was only travelling half
			transform->SetRotation({ transform->GetRotation().x, transform->GetRotation().y, (ratio * radians) + (yRatio * yRadians) });
			//transform->SetRotation({ transform->GetRotation().x, transform->GetRotation().y, (ratio * degrees) + (yRatio * 45.0f) });

			//cout << (ratio * radians / XM_PI * 180.0f) + (yRatio * yRadians / XM_PI * 180.0f) << endl;
			//cout << ratio * (radians / XM_PI * 180.0f) << " " << yRatio * (yRadians / XM_PI * 180.0f) << endl;
			//cout << (ratio * degrees) << " " << (ratio * degrees) + (yRatio * 45.0f) << endl;
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