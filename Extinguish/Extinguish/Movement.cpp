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
		transform->Translate({ 0.0f, 0.0f, moveSpeed * dt });
		isMoving = true;
	}

	if (input->GetKey('K'))
	{
		transform->Translate({ 0.0f, 0.0f, -moveSpeed * dt });
		isMoving = true;
	}

	if (input->GetKey('J'))
	{
		transform->Translate({ -moveSpeed * dt, 0.0f, 0.0f });
		isMoving = true;
	}

	if (input->GetKey('L'))
	{
		transform->Translate({ moveSpeed * dt, 0.0f, 0.0f });
		isMoving = true;
	}

	if (input->GetKey('U')) //up
	{
		transform->Translate({ 0.0f, moveSpeed * dt, 0.0f });
		isMoving = true;
	}

	if (input->GetKey('O')) //down
	{
		transform->Translate({ 0.0f, -moveSpeed * dt, 0.0f });
		isMoving = true;
	}
}

//getters
bool Movement::IsMoving()
{
	return isMoving;
}