#include "Movement.h"
#include "Transform.h"
#include "GameObject.h"

void Movement::Init(float moveVelocity, float rotateVelocity)
{
	//cache
	transform = GetGameObject()->GetTransform();

	moveSpeed = moveVelocity;
	rotateSpeed = rotateVelocity;
}

void Movement::Update(float dt, InputManager* input)
{
	if (input->GetKey('I'))
	{
		XMFLOAT3 forward = transform->GetForward();
		transform->Translate({ forward.x * moveSpeed * dt, forward.y * moveSpeed * dt,  forward.z * moveSpeed * dt });
		isMoving = true;
	}

	if (input->GetKey('K'))
	{
		//transform->Translate({ 0.0f, 0.0f, -moveSpeed * dt });
		XMFLOAT3 forward = transform->GetForward();
		transform->Translate({ forward.x * -moveSpeed * dt, forward.y * -moveSpeed * dt,  forward.z * -moveSpeed * dt });
		isMoving = true;
	}

	if (input->GetKey('J'))
	{
		XMFLOAT3 right = transform->GetRight();
		transform->Translate({ right.x * -moveSpeed * dt, right.y * -moveSpeed * dt,  right.z * -moveSpeed * dt });
		isMoving = true;
	}

	if (input->GetKey('L'))
	{
		XMFLOAT3 right = transform->GetRight();
		transform->Translate({ right.x * moveSpeed * dt, right.y * moveSpeed * dt,  right.z * moveSpeed * dt });
		isMoving = true;
	}

	if (input->GetKey('U')) //up
	{
		XMFLOAT3 up = transform->GetUp();
		transform->Translate({ up.x * moveSpeed * dt, up.y * moveSpeed * dt,  up.z * moveSpeed * dt });
		isMoving = true;
	}

	if (input->GetKey('P')) //down
	{
		XMFLOAT3 up = transform->GetUp();
		transform->Translate({ up.x * -moveSpeed * dt, up.y * -moveSpeed * dt,  up.z * -moveSpeed * dt });
		isMoving = true;
	}
}

//getters
bool Movement::IsMoving()
{
	return isMoving;
}