#pragma once
#include "Component.h"
#include "GameObject.h"
#include "vec3.h"

class BallController : public Component
{
private:
	bool isHeld = false;
	GameObject *me = GetGameObject();
	GameObject *holder;


public:
	void Init();
	void Update();
	//void Collide(Collider *obj);

	// actions
	void ThrowTo(GameObject *target);

	// accessors
	bool GetIsHeld();
	GameObject* GetHolder();

	// mutators
	void SetIsHeld(bool ans);
	void SetHolder(GameObject *person);
};