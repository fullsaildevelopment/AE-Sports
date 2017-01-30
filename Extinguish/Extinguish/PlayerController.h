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

	int prevMouseX, prevMouseY;

	//private helper functions
	void MovePlayer(float dt);
	//void ClampToCamera();

public:
	PlayerController();
	~PlayerController();

	//basic
	void Init();
	void Update(float dt) override;
};