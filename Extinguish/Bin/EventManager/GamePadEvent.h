#pragma once
#include "Event.h"

class GamePadEvent : public Event
{
private:
	void* gamePadState;

public:
	void Init();

	void Init(void* state);

	//getters//
	void* GetState();
};