#pragma once
#include "GameObject.h"
#include "Component.h"
#include "CapsuleCollider.h"
#include "SphereCollider.h"
#include "vec3.h"
#include "XTime.h"

class BallController : public Component
{
private:
	bool isHeld = false;
	bool isThrown = false;
	GameObject *me;
	GameObject *holder;
	XTime timer;


public:
	BallController(GameObject* obj);
	void Init();
	void Update(float dt, InputManager* input) override;
	void OnTriggerEnter(Collider *obj) override;

	// actions
	void ThrowTo(GameObject *target);
	void DropBall(GameObject *holder);
	void Throw();

	// accessors
	bool GetIsHeld();
	GameObject* GetHolder();

	// mutators
	void SetIsHeld(bool ans);
	void SetHolder(GameObject *person);
};