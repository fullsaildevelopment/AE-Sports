#pragma once
#include <DirectXMath.h>
#include "Component.h"
#include "InputManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "InputDownEvent.h"

class Camera : public Component
{
private:
	//Transform* camTransform;
	InputManager* input;
	Transform* transform;
	Transform* playerTransform;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT3 clampPos;

	float moveSpeed;
	float rotateSpeed;
	int prevMouseX, prevMouseY;
	float dt;

	float curRotX, curRotY;
	const float maxRotX, maxRotY;

	//private helper functions
	void MoveCamera(InputDownEvent* e);
	void ClampTo();
public:
	Camera();
	~Camera();

	//basic
	void Init(XMVECTORF32 eye, XMVECTORF32 at, XMVECTORF32 up, float moveVel, float rotateVel);
	void Update(float dt, InputManager* input) override;
	void HandleEvent(Event* e);
	void UpdateCamsRotation(float x, float y);

	//setters
	//void SetClampMatrix(DirectX::XMFLOAT4X4 matrix);
	void SetClampPos(DirectX::XMFLOAT3 pos);

	//getters
	XMFLOAT4X4 GetView();
};