#include "InputDownEvent.h"

//InputDownEvent::InputDownEvent(int key, int mouse)
//InputDownEvent::InputDownEvent(InputManager* inputManager, bool isServer)
//{
//	input = inputManager;
//	this->isServer = isServer;
//}

InputDownEvent::InputDownEvent(InputManager* inputManager, int id)
{
	input = inputManager;
	clientID = id;
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

//bool InputDownEvent::IsServer()
//{
//	return isServer;
//}

//setters//
//void InputDownEvent::SetIsServer(bool toggle)
//{
//	isServer = toggle;
//}

void InputDownEvent::SetID(int id)
{
	clientID = id;
}