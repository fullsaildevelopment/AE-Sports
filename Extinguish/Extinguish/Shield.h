#pragma once
#include "PowerUp.h"

class Shield : public PowerUp
{
private:

public:
	//structors
	Shield();

	//basic
	void Activate() override;
	void Deactivate() override;
};