#pragma once
#include "Event.h"

class SoundEvent : public Event
{
private:
	unsigned int soundID;
	unsigned int objectID;

public:
	//no need to implement
	void Init() override;
	void Init(unsigned int soundID, unsigned int objectID);

	//getters//
	unsigned int GetSoundID();
	unsigned int GetObjectID();
};