#pragma once
#include "Component.h"
#include "InputManager.h"
#include "GameObject.h"
#include "Transform.h"

class Camera;

class PlayerController : public Component
{
private:
	InputManager* input;
	Transform* transform;
	Camera* camera;
	Transform* cameraTransform;
	float moveSpeed;
	float rotateSpeed;
	int prevMouseX, prevMouseY;

	//private helper functions
	void MovePlayer(float dt);
	//void ClampToCamera();

public:
	PlayerController();
	~PlayerController();

	//basic
	void Init(float moveVelocity, float rotateVelocity);
	void Update(float dt, InputManager* input) override;
};