#pragma once
#include "Event.h"
#include "InputManager.h"

class InputDownEvent : public Event
{
private:
	InputManager* input;

	//int keyPressed;
	//int mouseButton; //-1 = left... 0 = middle... 1 = right
	//int x, y;

public:
	InputDownEvent(InputManager* inputManager);
	//InputDownEvent(int key, int mouse);
	~InputDownEvent();

	void Init() override;

	//getters
	InputManager* GetInput();
	//int GetKey();
	//int GetMouse();
	//int GetX();
	//int GetY();
};