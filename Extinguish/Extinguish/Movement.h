#pragma once
#include "Component.h"

class InputManager;
class Transform;
class InputDownEvent;
class GamePadEvent;

class Movement : public Component
{
private:
	Transform* transform;
	float moveSpeed;
	float rotateSpeed;
	bool isMoving;
	float _dt;
	char forward, back, left, right, up, down;
	bool canMove;

	//bool footstepsPlayed;
	//float timeSincePlayed;

	//private helper functions
	void HandleInput(InputDownEvent* e);
	void HandleGamePad(GamePadEvent* gamePadEvent);

public:
	//basic
	void Init(float moveVelocity, float rotateVelocity);
	void Update(float _dt) override;
	void HandleEvent(Event* e) override;
	//void ToggleFootstepsSound(int sound);

	//getters
	bool IsMoving();
	bool CanMove();

	//setters//
	void SetKeys(char forward, char back, char left, char right, char up, char down);
	void SetCanMove(bool toggle);
};