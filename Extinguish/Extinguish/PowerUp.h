#pragma once
#include "Component.h"

class PowerUp : public Component
{
private:
	enum POWERUP_TYPE {
		NOTHING,
		LOCK_ON,
		SUPER_JUMP,
		MAGNET,
		SPEED_BOOST,
		SHIELD_BUBBLE,
		ROCKET_BOOTS,
		WALL_RUNNER
	};

	POWERUP_TYPE type; // for the function pointers
	// tho if it's a base class, then... i dunno. :D
	float cooldown; // how long until you can use the powerup again
	float duration; // how long the powerup lasts for
public:

	void Update(float _dt) override;
	void Shutdown() override;
	void HandleEvent(Event* e) override;

};