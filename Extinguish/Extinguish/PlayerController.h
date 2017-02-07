#pragma once
#include "Component.h"
#include "InputManager.h"
#include "Transform.h"

class GameObject;
class Camera;

class PlayerController : public Component
{
private:
	//temp cache
	InputManager* input; 
	Transform* otherPlayer; //is set when colliding with other player

	//cache
	Transform* transform;
	Camera* camera;
	Transform* cameraTransform;

	float dt;
	

	//private helper functions
	void HandleInput();

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
};