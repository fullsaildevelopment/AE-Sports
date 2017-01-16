#pragma once
#include "Component.h"
#include "GameObject.h"
#include "vec3.h"

class Ball : public Component
{
private:
	bool isHeld = false;
	
public:
	void Throw();

	//accessors
	bool GetIsHeld();

	//mutators
	void SetIsHeld();
};