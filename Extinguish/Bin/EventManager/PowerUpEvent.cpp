#include "PowerUpEvent.h"

//structors//
PowerUpEvent::PowerUpEvent(std::string name, bool active, int spawnInd, int posInd)
{
	SetIsActive(active);
	SetName(name);
	SetSpawnIndex(spawnInd);
	SetPosIndex(posInd);
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

int PowerUpEvent::GetSpawnIndex()
{
	return spawnIndex;
}

int PowerUpEvent::GetPosIndex()
{
	return posIndex;
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

void PowerUpEvent::SetSpawnIndex(int index)
{
	spawnIndex = index;
}

void PowerUpEvent::SetPosIndex(int index)
{
	posIndex = index;
}