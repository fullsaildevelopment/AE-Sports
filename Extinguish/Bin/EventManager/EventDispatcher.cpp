#include "EventDispatcher.h"

EventDispatcher *EventDispatcher::singleton = 0;

EventDispatcher::~EventDispatcher()
{
	delete singleton;
}

//basic
//basic
void EventDispatcher::RegisterHandler(EventHandler* handler)
{
	handlers.push_back(handler);
}

//sends event to all devices registere
void EventDispatcher::Dispatch(Event* event)
{
	for (int i = 0; i < handlers.size(); ++i)
	{
		handlers[i]->HandleEvent(event);
	}
}

//getters
EventDispatcher* EventDispatcher::GetSingleton()
{
	if (!singleton)
	{
		singleton = new EventDispatcher();
	}

	return singleton;
}