#pragma once
#include "PowerUp.h"

class BallController;

class Magnet : public PowerUp
{
private:
	BallController* ballC;
	float originalMultiplier;

	//const members
	const float magnetRangeMultiplier = 10.0f;
public:
	//structors
	Magnet();

	//basic
	void Init(GameObject* obj) override;
	void Activate() override;
	void Deactivate() override;
};