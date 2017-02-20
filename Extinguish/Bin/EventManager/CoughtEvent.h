#pragma once
#include "Event.h"

class CoughtEvent : public Event
{
public:
	int id;
	bool holding;
	void  Init() override {};
};