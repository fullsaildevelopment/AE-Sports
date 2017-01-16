#pragma once
#include "Component.h"
#include "InputManager.h"
#include "GameObject.h"
#include "PlayerController.h"

class Crosse : public Component
{
private:
	Transform* transform;
	InputManager* input;

	int prevMouseX, prevMouseY;
	const float maxRotZ = 60.0f;

	//private helper functions
	void HandleInput(float dt);
	void ClampToPlayer();
public:
	Crosse();
	~Crosse();

	//basic
	void Init();
	void Update(float dt, InputManager* input) override;
};