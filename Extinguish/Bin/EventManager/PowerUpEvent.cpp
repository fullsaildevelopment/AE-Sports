#include "PowerUpEvent.h"

//structors//
PowerUpEvent::PowerUpEvent(std::string name, bool active, int index)
{
	SetIsActive(active);
	SetName(name);
	SetManagerIndex(index);
}

//basic//
void PowerUpEvent::Init()
{
	//do nothing
}

//getters//
std::string PowerUpEvent::GetName()
{
	return name;
}

bool PowerUpEvent::IsActive()
{
	return isActive;
}

int PowerUpEvent::GetManagerIndex()
{
	return managerIndex;
}

//setters//
void PowerUpEvent::SetName(std::string name)
{
	this->name = name;
}

void PowerUpEvent::SetIsActive(bool active)
{
	isActive = active;
}

void PowerUpEvent::SetManagerIndex(int index)
{
	managerIndex;
}