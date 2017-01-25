#pragma once
#include "EventHandler.h"
#include <vector>

class EventDispatcher
{
private:
	static EventDispatcher* singleton;
	std::vector<EventHandler*> handlers;

public:
	~EventDispatcher();

	//basic
	void RegisterHandler(EventHandler* handler);

	//sends event to all devices registere
	void Dispatch(Event* event);

	//getters
	static EventDispatcher* GetSingleton();
};