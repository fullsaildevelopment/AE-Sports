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
	if (handlerName == "" || handlersTable.IsUsed(handlerName))
		float temp = 0.0f;
	if (!handlersTable.IsUsed(handlerName))
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

//this will dispatch it to every single handler except the one specified
void EventDispatcher::DispatchExcept(Event* event, std::string handlerName)
{
	int indexToAvoid = handlersTable.GetKey(handlerName);

	for (int i = 0; i < handlers.size(); ++i)
	{
		if (i != indexToAvoid)
		{
			handlers[i]->HandleEvent(event);
		}
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

void EventDispatcher::RemoveHandler(std::string handleName)
{
	//int index = handlersTable.GetKey(handleName);
	//EventHandler * even = handlers[index];
	if (even)
	{
		handlers.erase(handlers.begin() + index);
	//	delete even;
	}
}