#include "SoundEvent.h"

//no need to implement
void SoundEvent::Init()
{

}

void SoundEvent::Init(unsigned int soundID, unsigned int objectID)
{
	this->soundID = soundID;
	this->objectID = objectID;
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