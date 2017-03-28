#pragma once
#include "PowerUp.h"

class Magnet : public PowerUp
{
private:

public:
	//structors
	Magnet();

	//basic
	void Activate() override;
	void Deactivate() override;
};