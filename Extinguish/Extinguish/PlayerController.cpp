#include "PlayerController.h"

PlayerController::PlayerController()
{

}

PlayerController::~PlayerController()
{

}

void PlayerController::Update(float dt, InputManager* input)
{
	this->input = input;

	HandleInput();

	//GetGameObject()->GetTransform()->Translate({ 0.01f, 0, 0 });
	//GetGameObject()->GetTransform()->RotateY(0.005f);
}

//setters


//private helper functions
void PlayerController::HandleInput()
{

}