#pragma once
#include <vector>
#include <string>
#include "EventHandler.h"
#include "HashStringCopy.h"

class EventDispatcher
{
private:
	static EventDispatcher* singleton;
	std::vector<EventHandler*> handlers;
	HashStringCopy handlersTable;

public:
	~EventDispatcher();

	//basic
	void RegisterHandler(EventHandler* handler, std::string handlerName);

	//sends event to all devices registere
	void Dispatch(Event* event);
	void DispatchTo(Event* event, std::string handlerName);

	//getters
	static EventDispatcher* GetSingleton();
};