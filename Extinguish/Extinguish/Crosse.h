#pragma once
#include "Component.h"
#include "InputManager.h"
#include "GameObject.h"
#include "PlayerController.h"
#include "EventHandler.h"
#include "InputDownEvent.h"

class Crosse : public Component
{
private:
	Transform* transform;
	Transform* ballTransform;
	InputManager* input;

	int prevMouseX, prevMouseY;
	const float maxRotZ = 70.0f;
	//const float maxX = 33.0f;
	//const float maxY = 35.0f;
	const float maxX = 43.0f;
	const float maxY = 46.0f;
	float dt;

	//private helper functions
	void HandleInput(InputDownEvent* e);

public:
	Crosse();
	~Crosse();

	//basic
	void Init();
	void Update(float dt, InputManager* input) override;

	//misc
	void Throw();
	void HandleEvent(Event* e) override;

};