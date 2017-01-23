#include "Movement.h"
#include "Transform.h"
#include "GameObject.h"
#include "InputDownEvent.h"
#include "EventDispatcher.h"

void Movement::Init(float moveVelocity, float rotateVelocity)
{
	//cache
	transform = GetGameObject()->GetTransform();

	moveSpeed = moveVelocity;
	rotateSpeed = rotateVelocity;

	//register movement event handler
	EventDispatcher::GetSingleton()->RegisterHandler(this);
}

void Movement::Update(float dt, InputManager* input)
{
	this->dt = dt;
}

void Movement::HandleEvent(Event* e)
{
	//filter events
	InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

	if (inputDownEvent)
	{
		HandleInput(inputDownEvent);
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

	if (input->GetKey('I'))
	{
		XMFLOAT3 forward = transform->GetForward();
		//transform->AddVelocity({ 0.0f, 0.0f, moveSpeed});
		transform->Translate({ forward.x * moveSpeed * dt, forward.y * moveSpeed * dt,  forward.z * moveSpeed * dt });
		isMoving = true;
	}

	if (input->GetKey('K'))
	{
		//transform->Translate({ 0.0f, 0.0f, -moveSpeed * dt });
		XMFLOAT3 forward = transform->GetForward();
		transform->Translate({ forward.x * -moveSpeed * dt, forward.y * -moveSpeed * dt,  forward.z * -moveSpeed * dt });
		//transform->AddVelocity({ 0.0f, 0.0f, -moveSpeed});
		isMoving = true;
	}

	if (input->GetKey('J'))
	{
		XMFLOAT3 right = transform->GetRight();
		transform->Translate({ right.x * -moveSpeed * dt, right.y * -moveSpeed * dt,  right.z * -moveSpeed * dt });
		//transform->AddVelocity({ -moveSpeed, 0.0f, 0.0f });
		isMoving = true;
	}

	if (input->GetKey('L'))
	{
		XMFLOAT3 right = transform->GetRight();
		transform->Translate({ right.x * moveSpeed * dt, right.y * moveSpeed * dt,  right.z * moveSpeed * dt });
		//transform->AddVelocity({ moveSpeed, 0.0f, 0.0f });
		isMoving = true;
	}

	if (input->GetKey('U')) //up
	{
		XMFLOAT3 up = transform->GetUp();
		transform->Translate({ up.x * moveSpeed * dt, up.y * moveSpeed * dt,  up.z * moveSpeed * dt });
		//transform->AddVelocity({ 0.0f, moveSpeed, 0.0f });
		isMoving = true;
	}

	if (input->GetKey('O')) //down
	{
		XMFLOAT3 up = transform->GetUp();
		transform->Translate({ up.x * -moveSpeed * dt, up.y * -moveSpeed * dt,  up.z * -moveSpeed * dt });
		//transform->AddVelocity({ 0.0f, -moveSpeed, 0.0f });
		isMoving = true;
	}
}