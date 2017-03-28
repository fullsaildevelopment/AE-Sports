#pragma once
#include "Component.h"

class Event;
class Collider;
class PlayerController;

class PowerUp : public Component
{
private:
	//enum POWERUP_TYPE {
	//	NOTHING,
	//	//LOCK_ON,
	//	SUPER_JUMP,
	//	MAGNET,
	//	SPEED_BOOST,
	//	SHIELD_BUBBLE,
	//	ROCKET_BOOTS,
	//	//WALL_RUNNER
	//};

	//POWERUP_TYPE type; // for the function pointers
	// tho if it's a base class, then... i dunno. :D
	//Tom: I like the idea of this being a base class

	//float cooldown; // how long until you can use the powerup again
	float duration; // how long the powerup lasts for
	float timer;
	bool isActivated;

	PlayerController* player;
public:
	//structors
	PowerUp();

	//basic
	void Update(float _dt) override;
	void Shutdown() override;
	void HandleEvent(Event* e) override;
	void OnTriggerEnter(Collider* collider) override;

	//misc
	virtual void Activate();
	virtual void Deactivate();

	//getters
	//float GetCooldown();
	float GetDuration();
	bool IsDone();
	PlayerController* GetPlayer();

	//setters
	//void SetCooldown(float down);
	void SetDuration(float dur);
};