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

	// for lerp
	float maxTime = 10.0f;
	float curTime = 0.0f;
	bool lerp = false;
	float3 destination;

	//private helper functions
	void MoveCamera(InputDownEvent* e);
	void MoveCamera(GamePadEvent* e);
	void LerpCamera(float dt);
public:
	Camera();
	~Camera();

	//basic
	void Init(XMVECTORF32 eye, XMVECTORF32 at, XMVECTORF32 up, float moveVel, float rotateVel, bool isEventHandler);
	void Update(float _dt) override;
	void HandleEvent(Event* e);

	//misc
	void LookAt(XMFLOAT3 pos, float totalTime);
	void MoveTo(XMFLOAT3 pos, float totalTime);

	//getters
	XMFLOAT4X4 GetView();

	// setters
	void StartLerp() { lerp = true; }
	void SetDestination(float3 des);
};