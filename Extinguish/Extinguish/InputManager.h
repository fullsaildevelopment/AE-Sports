#pragma once
#include "Includes.h"
//#include <Windows.h>
#include <windowsx.h>

class InputManager
{
private:
	enum KeyState
	{
		NOTPRESSED,
		DOWN,
		UP,
		HELD,
	};

	static InputManager* input;

	//KeyState keyboard[256];
	//bool rightButtonPressed, leftButtonPressed, middleButtonPressed;
	bool keyboard[256];
	bool keyboardDown[256];
	bool keyboardUp[256];
	bool mouse[3];
	bool mouseDown[3];
	bool mouseUp[3];
	int mouseX, mouseY;
	//KeyState mouseButtons[3];

	bool alreadySent;

	//private helper functions
	void SendEvent();

public:
	~InputManager();

	//basic
	void ClearKeyboard();
	void Init(bool keyboard[256], bool keyboardDown[256], bool keyboardUp[256], bool mouse[3], bool mouseDown[3], bool mouseUp[3], int mouseX, int mouseY); //only to be used for events
	void Update();
	void Shutdown();

	void WindowResize(uint16_t w, uint16_t h);

	//getters
	static InputManager* GetSingleton();
	bool GetKey(unsigned int button);
	bool GetKeyDown(unsigned int button);
	bool GetKeyUp(unsigned int button);
	int GetMouseX();
	int GetMouseY();
	bool GetMouseButton(int index);
	bool GetMouseButtonDown(int index);
	bool GetMouseButtonUp(int index);
	bool* GetKeyboard();
	bool* GetKeyboardDown();
	bool* GetKeyboardUp();
	bool* GetMouse();
	bool* GetMouseDown();
	bool* GetMouseUp();
	
	//setters
	void SetKeyboardKey(unsigned int index, bool toggle);
	void SetMouseButtons(int index, bool toggle);
	void SetMousePosition(LPARAM lparam);
};