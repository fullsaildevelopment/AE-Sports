#pragma once
#include "Component.h"
//#include "vec3.h"

class GameObject;
class Camera;
class Transform;
class GamePadEvent;

class PlayerController : public Component
{
private:
	//temp cache
	InputManager* input;
	GameObject* otherPlayer; //is set when colliding with other player
	GameObject* floor;

	//cache
	Transform* transform;
	Camera* camera;
	Transform* cameraTransform;
	
	//reg members
	float dt;
	bool justJumped;
	bool isSprinting;
	bool isCharging;
	float canSprint;
	float originalMaxSpeed;
	float chargeTimer;
	bool footstepsPlayed;
	//AkUniqueID stepsPlayID;
	//AkUniqueID stepsStopID;
	int footstepsSound;
	//float sprintAgainTimer;
	//float3 originalVelocity;

	//const members
	//const float chargeSpeed = 150.0f;
	const float sprintMultiplier = 2.5f;// 1.75f;
	const float chargeMultiplier = 2.25f;
	const float timeTilCharge = 2.5f;
	//const float timeTilSprint = 3.0f;
	const float sprintCost = 0.15f;

	//private helper functions
	void HandleInput();
	void HandleGamePad(GamePadEvent* gamePadEvent);
	void HandleSprintAndCharge();
	void PlayFootstepsSound();
	void StopFootstepsSound();
	void SetFootstepsSound(int sound);
public:
	PlayerController();
	~PlayerController();

	//basic
	void Init();
	void Update(float dt) override;
	void HandleEvent(Event* e) override;

	//misc
	void OnCollisionEnter(Collider* collider) override;
	void OnCollisionExit(Collider* collider) override;

	//actions
	void Jump();
	void Attack();
	void Sprint();
};