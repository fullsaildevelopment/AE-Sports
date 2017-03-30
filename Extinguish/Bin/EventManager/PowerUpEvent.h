#pragma once
#include <string>
#include "Event.h"

//this is to tell the recipient what powerup it is and whether it's becoming active or inactive

class PowerUpEvent : public Event
{
private:
	bool isActive;
	std::string name;
	int spawnIndex;
	int posIndex;

public:
	//structors
	PowerUpEvent(std::string name, bool active, int spawnInd, int posInd);

	//basic
	void Init() override;

	//getters
	std::string GetName();
	bool IsActive();
	int GetSpawnIndex();
	int GetPosIndex();

	//setters
	void SetName(std::string name);
	void SetIsActive(bool active);
	void SetSpawnIndex(int index);
	void SetPosIndex(int index);
};