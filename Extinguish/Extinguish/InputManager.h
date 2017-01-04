#pragma once
#include <Windows.h>
#include <windowsx.h>

class InputManager
{
private:
	enum KeyState
	{
		DOWN = 0, 
		UP,
		HELD,
		NOTPRESSED,
	};

	KeyState keyboard[256];
	int mouseX, mouseY;
	bool rightButtonPressed, leftButtonPressed;

public:
	void ClearKeyboard();

	//getters
	bool GetKey(unsigned int button);
	bool GetKeyDown(unsigned int button);
	int GetMouseX();
	int GetMouseY();
	bool GetMouseButton(int index);

	//I've taken out buttonUp because it's not necessarily needed, and I didn't know hot to implement without wasting
	//processing time or memory 
	//bool GetButtonUp(unsigned int button);
	
	//setters
	void SetKeyboardKey(unsigned int index, bool toggle);
	void SetMouseButtons(int index, bool toggle);
	void SetMousePosition(LPARAM lparam);
};