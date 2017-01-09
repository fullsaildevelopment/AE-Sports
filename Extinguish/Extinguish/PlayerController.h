#pragma once
#include "Component.h"
#include "InputManager.h"
#include "GameObject.h"
#include "Transform.h"

class PlayerController : public Component
{
private:
	InputManager* input;
	DirectX::XMFLOAT4X4 camera;

	//private helper functions
	void HandleInput();
	void ClampToCamera();

public:
	PlayerController();
	~PlayerController();

	//basic
	void Update(float dt, InputManager* input) override;
	void SetCameraWorld(DirectX::XMFLOAT4X4 cam);
};