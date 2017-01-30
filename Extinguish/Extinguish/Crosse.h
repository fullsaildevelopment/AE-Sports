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
	float minX, minY;
	float dt;

	int state; //0 = default... 1 = catch... 2 = default to catch... 3 = catch to default...

	//private helper functions
	void HandleInput(InputDownEvent* e);

public:
	Crosse();
	~Crosse();

	//basic
	void Init();
	void Update(float dt, InputManager* input) override;
	void OnTriggerEnter(Collider* collider) override;

	//misc
	void Throw();
	void HandleEvent(Event* e) override;

	//setters//
	void SetHolder(GameObject* object);

};