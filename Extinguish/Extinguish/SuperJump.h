#pragma once
#include "PowerUp.h"

class SuperJump : public PowerUp
{
private:
	float originalMultiplier;
	std::string name = "SuperJump";

	//const members
	const float jumpMultiplier = 2.0f;

public:
	//structors
	SuperJump();

	//basic
	void Activate() override;
	void Deactivate() override;

	//getters
	std::string GetName() override;
};