#include "InputDownEvent.h"

//InputDownEvent::InputDownEvent(int key, int mouse)
//InputDownEvent::InputDownEvent(InputManager* inputManager, bool isServer)
//{
//	input = inputManager;
//	this->isServer = isServer;
//}

InputDownEvent::InputDownEvent()
{
	input = nullptr;
	clientID = -1;
	isServer = false;
}

InputDownEvent::InputDownEvent(InputManager* inputManager, int id, bool server)
{
	input = inputManager;
	clientID = id;
	isServer = server;
}

InputDownEvent::~InputDownEvent()
{

}

//no need to implement
void InputDownEvent::Init()
{

}

//getters//
InputManager* InputDownEvent::GetInput()
{
	return input;
}

int InputDownEvent::GetID()
{
	return clientID;
}

bool InputDownEvent::IsServer()
{
	return isServer;
}

//setters//
void InputDownEvent::SetID(int id)
{
	clientID = id;
}

void InputDownEvent::SetIsServer(bool toggle)
{
	isServer = toggle;
}

void InputDownEvent::SetInput(InputManager* inputManager)
{
	input = inputManager;
}