#pragma once
#include "Component.h"
#include "GameObject.h"
#include "vec3.h"
#include "SphereCollider.h"

class BallController : public Component
{
private:
	bool isHeld = false;
	GameObject *me = GetGameObject();
	GameObject *holder;


public:
	void Init();
	void Update();
	void OnTriggerEnter(Collider *obj) override;

	// actions
	void ThrowTo(GameObject *target);
	void DropBall(GameObject *holder);

	// accessors
	bool GetIsHeld();
	GameObject* GetHolder();

	// mutators
	void SetIsHeld(bool ans);
	void SetHolder(GameObject *person);
};