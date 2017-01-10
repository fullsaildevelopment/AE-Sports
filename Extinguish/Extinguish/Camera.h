#pragma once
#include <DirectXMath.h>
#include "Component.h"
#include "InputManager.h"
#include "GameObject.h"
#include "Transform.h"

class Camera : public Component
{
private:
	InputManager* input;
	Transform* transform;
	DirectX::XMFLOAT4X4 clamp;

	float moveSpeed;
	float rotateSpeed;
	int prevMouseX, prevMouseY;

	//private helper functions
	void MoveCamera(float dt);
	void ClampTo();
public:
	//basic
	void Init(XMVECTORF32 eye, XMVECTORF32 at, XMVECTORF32 up, float moveVel, float rotateVel);
	void Update(float dt, InputManager* input) override;

	//setters
	void SetClampMatrix(DirectX::XMFLOAT4X4 matrix);
};