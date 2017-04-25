#include "SoundEvent.h"

//no need to implement
void SoundEvent::Init()
{

}

void SoundEvent::Init(unsigned int soundID, unsigned int objectID, bool playNetworked)
{
	this->soundID = soundID;
	this->objectID = objectID;
	this->playNetworked = playNetworked;
}

//getters//
unsigned int SoundEvent::GetSoundID()
{
	return soundID;
}

unsigned int SoundEvent::GetObjectID()
{
	return objectID;
}

bool SoundEvent::PlayNetworked()
{
	return playNetworked;
}