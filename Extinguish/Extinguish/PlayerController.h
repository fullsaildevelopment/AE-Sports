#pragma once
#include "Component.h"

class GameObject;
class Camera;
class Transform;
class InputManager;

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

	float dt;
	bool justJumped;

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