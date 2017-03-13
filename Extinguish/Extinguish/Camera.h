#pragma once
#include <DirectXMath.h>
#include "Component.h"
#include "Transform.h"
#include "InputManager.h"
#include "InputDownEvent.h"

class GameObject;
class GamePadEvent;

class Camera : public Component
{
private:
	//Transform* camTransform;
	InputManager* input;
	Transform* transform;
	Transform* playerTransform;
	DirectX::XMFLOAT4X4 view;

	float moveSpeed;
	float rotateSpeed;
	int prevMouseX, prevMouseY;
	float _dt;
	float sensitivityX, sensitivityY;
	const float padSensitivityX, padSensitivityY;

	float curRotX, curRotY;
	const float maxRotX, maxRotY;

	//private helper functions
	void MoveCamera(InputDownEvent* e);
	void MoveCamera(GamePadEvent* e);
public:
	Camera();
	~Camera();

	//basic
	void Init(XMVECTORF32 eye, XMVECTORF32 at, XMVECTORF32 up, float moveVel, float rotateVel, bool isEventHandler);
	void Update(float _dt) override;
	void HandleEvent(Event* e);

	//getters
	XMFLOAT4X4 GetView();
};