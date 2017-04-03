#pragma once
#include "PowerUp.h"

class BallController;
class Crosse;

class Magnet : public PowerUp
{
private:
	//object members
	Crosse* crosseC;

	//basic members
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