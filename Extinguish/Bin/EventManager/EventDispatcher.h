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
	void Shutdown();
	void ClearHandlers() { handlers.clear(); handlersTable.Clear(); }

	//sends event to all devices registere
	void Dispatch(Event* event);

	//send event to specific handler
	void DispatchTo(Event* event, std::string handlerName);

	//send event to all devices except this specific handler
	void DispatchExcept(Event* event, std::string handlerName);

	//getters
	static EventDispatcher* GetSingleton();
};