#pragma once
#include "Component.h"
#include "GameObject.h"
#include "vec3.h"

class Ball : public Component
{
private:
	bool isHeld = false;
	GameObject *me = GetGameObject();


public:
	void Init();
	void Update();

	// actions
	void ThrowTo(GameObject *target);

	// accessors
	bool GetIsHeld();

	// mutators
	void SetIsHeld(bool ans);
};