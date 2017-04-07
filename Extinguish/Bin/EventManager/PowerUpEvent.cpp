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

int PowerUpEvent::GetClientID()
{
	return clientID;
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

void PowerUpEvent::SetClientID(int id)
{
	clientID = id;
}