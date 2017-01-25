#pragma once
#include "Event.h"

class EventHandler
{
private:

public:
	//if a class needs to listen to event, it needs to inherit from eventhandler and override this function
	//have a switch statement inside
	virtual void HandleEvent(Event* e) = 0;
};