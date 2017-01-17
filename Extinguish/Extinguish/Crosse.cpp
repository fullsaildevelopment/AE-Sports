#include <iostream>
#include "Crosse.h"
#include "EventDispatcher.h"

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
	//ballTransform = GetGameObject()->FindGameObject("Ball")->GetTransform();

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

void Crosse::Throw()
{
	// do animation on crosse
	transform->RotateX(XMConvertToRadians(45));

	//add force to ball
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
	input = e->GetInput();

	//rotate the crosse
	if (input->GetMouseX() && input->GetMouseY())
	{
		if (input->GetMouseButton(0))
		{
			//cout << "Click!" << endl;
			float dx = (float)input->GetMouseX() - (float)prevMouseX;
			float dy = (float)input->GetMouseY() - (float)prevMouseY;

			float radX = dy * dt;
			float radY = dx * dt;

			float curZDeg = transform->GetRotation().z / XM_PI * 180;
			float degY = radY / XM_PI * 180;

			float rotateRad = radY;

			if (!(curZDeg + degY > maxRotZ || curZDeg + degY < -maxRotZ))
			{
				//cout << transform->GetRotation().x / XM_PI * 180 << " " << transform->GetRotation().y / XM_PI * 180 << " " << transform->GetRotation().z / XM_PI * 180 << endl;

				transform->RotateZ(rotateRad);

				//also translate up or down depending on rotateRad
				//XMFLOAT3 up = { transform->GetUp().x * 10, transform->GetUp().y * 10, transform->GetUp().z * 10 };
				XMFLOAT3 up = { 0.05f, 0.05f, 0.05f };

				//move the crosse to make it look better
				if (degY < 0.0f)
				{
					float moveX = (-maxX - transform->GetPosition().x) * (float)(degY / curZDeg) + transform->GetPosition().x;
					float moveY = (-maxY - transform->GetPosition().y) * (float)(degY / curZDeg) + transform->GetPosition().y;

					cout << moveX << " " << moveY << endl;
				}
				else if (degY > 0.0f)
				{
					float moveX = (maxX - transform->GetPosition().x) * (float)(degY / curZDeg) + transform->GetPosition().x;
					float moveY = (maxY - transform->GetPosition().y) * (float)(degY / curZDeg) + transform->GetPosition().y;

					cout << moveX << " " << moveY << endl;
				}

				//if ((rotateRad > 0.0f && transform->GetRotationDeg().z > 0.0f) || (rotateRad < 0.0f && transform->GetRotationDeg().z < 0.0f))
				//{
				//	transform->Translate(up);
				//	cout << "up" << endl;
				//}
				//else if (rotateRad != 0.0f)
				//{
				//	transform->Translate({ -up.x, -up.y, -up.z });
				//	cout << "down" << endl;
				//}
			}
		}

		prevMouseX = input->GetMouseX();
		prevMouseY = input->GetMouseY();
	}

	//cout << "Input!" << endl;

	if (input->GetMouseButtonDown(1))
	{
		Throw();
	}

	//if (input->GetKeyDown('Y'))
	//{
	//	cout << "It works!" << endl;
	//}

	//if (input->GetKey('H'))
	//{
	//	cout << "This should work more!" << endl;
	//}

	if (input->GetKey('T'))
	{
		transform->Translate(transform->GetUp());
		cout << transform->GetPosition().x << " " << transform->GetPosition().y << " " << transform->GetPosition().z << endl;
	}
}