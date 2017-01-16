#include "Crosse.h"
#include <iostream>

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
}

void Crosse::Update(float dt, InputManager* input)
{
	this->input = input;

	HandleInput(dt);

	//transform->RotateY(0.01f);

	//set crosse to be at player's position
	//ClampToPlayer();
}

//private helper functions//

void Crosse::HandleInput(float dt)
{
	//rotate the crosse
	if (input->GetMouseX() && input->GetMouseY())
	{
		if (input->GetMouseButton(-1))
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
				//cout << transform->GetRotation().x / XM_PI * 180 << " " << transform->GetRotation().y / XM_PI * 180 << " " << transform->GetRotation().z / XM_PI * 180 << endl;

				transform->RotateZ(rotateRad);

				//also translate up or down depending on rotateRad
				XMFLOAT3 up = transform->GetUp();

				if (rotateRad > 0.0f)
				{
					transform->Translate(up);
				}
				else
				{
					transform->Translate({ -up.x, -up.y, -up.z });
				}
			}
		}

		prevMouseX = input->GetMouseX();
		prevMouseY = input->GetMouseY();
	}

	//if (input->GetKey('T'))
	//{
	//	transform->Translate(transform->GetUp());
	//	cout << transform->GetPosition().x << " " << transform->GetPosition().y << " " << transform->GetPosition().z << endl;
	//}
}

void Crosse::ClampToPlayer()
{
	////get player's world
	Transform* playerTransform = GetGameObject()->GetTransform()->GetParent();

	////alter the world to suitable stick position
	XMFLOAT3 playerPos = playerTransform->GetPosition();
	////playerPos.y += 100;
	////playerPos.z += 1;

	////set crosse's pos to pos
	transform->SetPosition(playerPos);

	//adjust crosse's pos given it's attached already
	//GetGameObject()->GetTransform()->Translate({ 0, 0, 2.0f });
}