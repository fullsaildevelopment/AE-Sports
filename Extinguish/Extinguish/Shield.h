#pragma once
#include "PowerUp.h"

class Shield : public PowerUp
{
private:
	std::string name = "Shield";

public:
	//structors
	Shield();

	//basic
	void Activate() override;
	void Deactivate() override;
	std::string GetName() override;
};