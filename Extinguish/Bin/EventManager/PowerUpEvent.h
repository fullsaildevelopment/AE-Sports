#pragma once
#include <string>
#include "Event.h"

//this is to tell the recipient what powerup it is and whether it's becoming active or inactive

class PowerUpEvent : public Event
{
private:
	bool isActive;
	std::string name;
	int managerIndex;

public:
	//structors
	PowerUpEvent(std::string name, bool active, int index);

	//basic
	void Init() override;

	//getters
	std::string GetName();
	bool IsActive();
	int GetManagerIndex();

	//setters
	void SetName(std::string name);
	void SetIsActive(bool active);
	void SetManagerIndex(int index);
};