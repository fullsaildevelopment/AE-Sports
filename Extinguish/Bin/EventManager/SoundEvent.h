#pragma once
#include "Event.h"

class SoundEvent : public Event
{
private:
	unsigned int soundID;
	unsigned int objectID;
	bool playNetworked;

public:
	//no need to implement
	void Init() override;
	void Init(unsigned int soundID, unsigned int objectID, bool playNetworked = true); //I made playNetworked default to true because I added it late after implying it was true most of the time

	//getters//
	unsigned int GetSoundID();
	unsigned int GetObjectID();
	bool PlayNetworked();
};