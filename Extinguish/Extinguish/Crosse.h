#pragma once
#include "Component.h"
#include "InputManager.h"
#include "GameObject.h"
#include "PlayerController.h"

class Crosse : public Component
{
private:

	//private helper functions
	void HandleInput();
	void ClampToPlayer();
public:
	Crosse();
	~Crosse();

	//basic
	void Update(float dt, InputManager* input) override;
};