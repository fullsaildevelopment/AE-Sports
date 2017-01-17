#pragma once
#include "Component.h"
#include "GameObject.h"
#include "vec3.h"

class Ball : public Component
{
private:
	bool isHeld = false;
	GameObject *me = GetGameObject();
	GameObject *holder;


public:
	void Init();
	void Update();
	void OnCollisionEnter(Collider *e);

	// actions
	void ThrowTo(GameObject *target);

	// accessors
	bool GetIsHeld();

	// mutators
	void SetIsHeld(bool ans);
};