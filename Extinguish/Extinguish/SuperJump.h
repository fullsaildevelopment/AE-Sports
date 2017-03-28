#pragma once
#include "PowerUp.h"

class SuperJump : public PowerUp
{
private:
	float originalMultiplier;

	//const members
	const float jumpMultiplier = 2.0f;

public:
	//structors
	SuperJump();

	//basic
	void Activate() override;
	void Deactivate() override;

};