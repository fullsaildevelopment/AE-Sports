#pragma once
#include "Event.h"
#include "GamePad.h"

class GamePadEvent : public Event
{
private:
	DirectX::GamePad::State* gamePadState;
	int clientID;

public:
	void Init();

	void Init(DirectX::GamePad::State* state, int id);

	//getters//
	DirectX::GamePad::State* GetState();
	int GetClientID();
};