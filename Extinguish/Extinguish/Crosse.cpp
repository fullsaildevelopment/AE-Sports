#include <iostream>
#include "Crosse.h"
#include "EventDispatcher.h"
#include "SphereCollider.h"
#include "BallController.h"

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
}

void Crosse::Update(float dt, InputManager* input)
{
	//this->input = input;

	this->dt = dt;
	//HandleInput(dt);

	//transform->RotateY(0.01f);

	//set crosse to be at player's position
	//ClampToPlayer();

}

void Crosse::OnCollisionEnter(Collider* collider)
{
	if (collider->GetGameObject()->GetName() == "GameBall")
	{
		collider->GetGameObject()->GetTransform()->SetParent(transform);

		BallController* ballController = collider->GetGameObject()->GetComponent<BallController>();
		ballController->SetIsHeld(true);

		cout << "Crosse collision with gameball" << endl;
	}
	else
	{
		cout << "Crosse collision with something else" << endl;
	}
}

//misc
void Crosse::Throw()
{
	const float throwSpeed = 50.0f;

	//detach ball
	ballTransform->SetPosition({ ballTransform->GetWorld()._41, ballTransform->GetWorld()._42, ballTransform->GetWorld()._43 });
	ballTransform->SetParent(nullptr);
	transform->RemoveChild(ballTransform);

	// do animation on crosse
	transform->RotateX(XMConvertToRadians(45));

	//add force to ball
	XMFLOAT3 ballForward = ballTransform->GetForward();
	ballForward = { 0, 0, 1 };
	ballTransform->AddVelocity({ ballForward.x * throwSpeed, ballForward.y * throwSpeed, ballForward.z * throwSpeed });
}

void Crosse::HandleEvent(Event* e)
{
	//filter throw events to find right one
	InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

	if (inputDownEvent)
	{
		HandleInput(inputDownEvent);
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
			float dx = (float)input->GetMouseX() - (float)prevMouseX;
			float dy = (float)input->GetMouseY() - (float)prevMouseY;

			float radX = dy * dt;
			float radY = dx * dt;

			float curZDeg = transform->GetRotation().z / XM_PI * 180;
			float degY = radY / XM_PI * 180;

			float rotateRad = radY;

			if (!(curZDeg + degY > maxRotZ || curZDeg + degY < -maxRotZ))
			{
				transform->RotateZ(rotateRad);

				//also translate up or down depending on rotateRad
				//XMFLOAT3 up = { transform->GetUp().x * 10, transform->GetUp().y * 10, transform->GetUp().z * 10 };
				XMFLOAT3 up = { 0.01f, 0.01f, 0 };

				//move the crosse to make it look better
				//move down if I'm going left and on right side or if I'm going right and on left side 
				if ((degY < 0.0f && curZDeg > 0.0f) || (degY > 0.0f && curZDeg < 0.0f))
				{
					//float moveX = (-maxX - transform->GetPosition().x) * (float)(degY / -maxRotZ) + transform->GetPosition().x * dt;
					//float moveY = (-maxY - transform->GetPosition().y) * (float)(degY / -maxRotZ) + transform->GetPosition().y * dt;

	//				if (!((moveX + transform->GetPosition().x < -maxX) || (moveY + transform->GetPosition().y < -maxY)))
	//				{
	//					moveX = maxX - transform->GetPosition().x;
	//				}
	///*				else if (moveY + transform->GetPosition().y > maxY)
	//				{
	//					moveY = maxY - transform->GetPosition().y;
	//				}*/
	//				else
	//				{
	//					//cout << "Down" << endl;
	//					//transform->Translate({ -maxX * (degY / -maxRotZ) * 0.1f, -maxY * (degY / -maxRotZ) * 0.1f, 0 });
	//					//transform->Translate({ moveX, moveY, 0 });
	//				}

					if (!(position.x - up.x < minX || position.y - up.y < minY))
					{
						//transform->Translate({ -up.x, -up.y, -up.z });
					}

					cout << position.x << " " << position.y << " " << position.z << endl;

					//cout << up.x << " " << up.y << endl;
				}
				else if ((degY < 0.0f && curZDeg <= 0.0f) || (degY > 0.0f && curZDeg >= 0.0f))
				{
					//float moveX = (maxX - transform->GetPosition().x) * (float)(degY / maxRotZ) + transform->GetPosition().x;
					//float moveY = (maxY - transform->GetPosition().y) * (float)(degY / maxRotZ) + transform->GetPosition().y;

					////transform->Translate({ maxX * (degY / maxRotZ), maxY * (degY / maxRotZ), 0 });

					//if (moveX + transform->GetPosition().x > maxX)
					//{
					//	moveX = maxX - transform->GetPosition().x;
					//}
					//else if (moveY + transform->GetPosition().y > maxY)
					//{
					//	moveY = maxY - transform->GetPosition().y;
					//}
					//else
					//{
					//	//transform->Translate({ maxX * (degY / -maxRotZ) * 0.1f, maxY * (degY / -maxRotZ) * 0.1f, 0 });
					//	//transform->Translate({ moveX, moveY, 0 });
					//}

					// && !(position.x + up.x < minX || position.y + up.y < minY)
					if (!(position.x + up.x > maxX || position.y + up.y > maxY))
					{
						//transform->Translate({ up.x, up.y, up.z });
					}
					
					//cout << up.x << " " << up.y << endl;
					//cout << "Up" << endl;

					//cout << moveX << " " << moveY << " " << degY << endl;
				}
			}
		}

		prevMouseX = input->GetMouseX();
		prevMouseY = input->GetMouseY();
	}

	if (input->GetMouseButtonDown(1))
	{
		Throw();
	}

	if (input->GetKeyDown('T'))
	{
		XMFLOAT3 up = transform->GetUp();
		transform->Translate({ up.x * dt, up.y * dt,  up.z * dt });
		cout << transform->GetPosition().x << " " << transform->GetPosition().y << " " << transform->GetPosition().z << endl;
	}
}