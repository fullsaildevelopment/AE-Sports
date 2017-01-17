#include "InputDownEvent.h"

//InputDownEvent::InputDownEvent(int key, int mouse)
InputDownEvent::InputDownEvent(InputManager* inputManager)
{
	input = inputManager;
	//keyPressed = key;
	//mouseButton = mouse;
}

InputDownEvent::~InputDownEvent()
{

}

//no need to implement
void InputDownEvent::Init()
{

}

//getters
InputManager* InputDownEvent::GetInput()
{
	return input;
}

//int InputDownEvent::GetKey()
//{
//	return keyPressed;
//}
//
//int InputDownEvent::GetMouse()
//{
//	return mouseButton;
//}
//
//int InputDownEvent::GetX()
//{
//	return x;
//}
//
//int InputDownEvent::GetY()
//{
//	return y;
//}