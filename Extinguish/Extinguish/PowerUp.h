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

	float duration; // how long the powerup lasts for
	float timer;
	bool isActivated;
	int spawnIndex, posIndex; //the manager needs this to help spawn

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
	float GetDuration();
	bool IsDone();
	PlayerController* GetPlayer();
	int GetSpawnIndex();
	int GetPosIndex();

	//setters
	void SetDuration(float dur);
	void SetSpawnIndex(int index);
	void SetPosIndex(int index);
};