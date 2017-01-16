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
		transform->AddVelocity({ 0.0f, 0.0f, moveSpeed});
		isMoving = true;
	}

	if (input->GetKey('K'))
	{
		transform->AddVelocity({ 0.0f, 0.0f, -moveSpeed});
		isMoving = true;
	}

	if (input->GetKey('J'))
	{
		transform->AddVelocity({ -moveSpeed, 0.0f, 0.0f });
		isMoving = true;
	}

	if (input->GetKey('L'))
	{
		transform->AddVelocity({ moveSpeed, 0.0f, 0.0f });
		isMoving = true;
	}

	if (input->GetKey('U')) //up
	{
		transform->AddVelocity({ 0.0f, moveSpeed, 0.0f });
		isMoving = true;
	}

	if (input->GetKey('O')) //down
	{
		transform->AddVelocity({ 0.0f, -moveSpeed, 0.0f });
		isMoving = true;
	}
}

//getters
bool Movement::IsMoving()
{
	return isMoving;
}