#include "EventDispatcher.h"

using namespace std;

EventDispatcher *EventDispatcher::singleton = 0;

EventDispatcher::~EventDispatcher()
{

}

//basic//
void EventDispatcher::RegisterHandler(EventHandler* handler, string handlerName)
{
	handlers.push_back(handler);
	handlersTable.Insert(handlerName);
}

void EventDispatcher::Shutdown()
{
	delete singleton;
	singleton = nullptr;
}

//sends event to all devices registere
void EventDispatcher::Dispatch(Event* event)
{
	for (int i = 0; i < handlers.size(); ++i)
	{
		handlers[i]->HandleEvent(event);
	}
}

void EventDispatcher::DispatchTo(Event* event, std::string handlerName)
{
	int index = handlersTable.GetKey(handlerName);

	if (index != -1)
	{
		handlers[index]->HandleEvent(event);
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