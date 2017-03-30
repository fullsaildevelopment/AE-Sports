#pragma once
#include "Event.h"

class PulseFloorEvent : public Event
{
public:
	int floorIndex;
	void Init() override {};
};