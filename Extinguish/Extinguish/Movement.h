#pragma once
#include "Component.h"

class InputManager;
class Transform;
class InputDownEvent;

class Movement : public Component
{
private:
	Transform* transform;
	float moveSpeed;
	float rotateSpeed;
	bool isMoving;
	float dt;
	char forward, back, left, right, up, down;
	float timeSincePlayed;

	//private helper functions
	void HandleInput(InputDownEvent* e);

public:
	//basic
	void Init(float moveVelocity, float rotateVelocity);
	void Update(float dt, InputManager* input) override;
	void HandleEvent(Event* e);

	//getters
	bool IsMoving();

	//setters//
	void SetKeys(char forward, char back, char left, char right, char up, char down);
};