#pragma once
#include "Event.h"
#include "InputManager.h"

class InputDownEvent : public Event
{
private:
	InputManager* input;
	int clientID;
	//bool isServer;


public:
	//InputDownEvent(InputManager* inputManager, bool isServer);
	InputDownEvent(InputManager* inputManager, int id);
	~InputDownEvent();

	void Init() override;

	//getters
	InputManager* GetInput();
	int GetID();
	//bool IsServer();

	//setters
	//void SetIsServer(bool toggle);
	void SetID(int id);
};