#pragma once
#include "Component.h"

class InputManager;
class Transform;

class Movement : public Component
{
private:
	Transform* transform;

	float moveSpeed;
	float rotateSpeed;
	bool isMoving;

public:
	//basic
	void Init(float moveVelocity, float rotateVelocity);
	void Update(float dt, InputManager* input) override;

	//getters
	bool IsMoving();
};