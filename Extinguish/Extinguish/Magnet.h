#pragma once
#include "PowerUp.h"

class BallController;
class Crosse;

class Magnet : public PowerUp
{
private:
	//object members
	Crosse* crosseC;

	std::string name = "Magnet";

	//basic members
	float originalMultiplier;
	float originalSpeedMultiplier;


	//const members
	const float magnetRangeMultiplier = 10.0f;
	const float magnetSpeedMultiplier = 20.0f;
public:
	//structors
	Magnet();

	//basic
	void Init(GameObject* obj) override;
	void Activate() override;
	void Deactivate() override;
	std::string GetName() override;
};