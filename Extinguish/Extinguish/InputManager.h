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
	bool keyboard[256];
	bool keyboardDown[256];
	bool keyboardUp[256];
	int mouseX, mouseY;
	//bool rightButtonPressed, leftButtonPressed, middleButtonPressed;
	KeyState mouseButtons[3];

	bool sendEvent;

	//private helper functions
	void SendEvent();

public:
	~InputManager();

	//basic
	void ClearKeyboard();
	void Update();

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

	//I've taken out buttonUp because it's not necessarily needed, and I didn't know hot to implement without wasting
	//processing time or memory 
	//bool GetButtonUp(unsigned int button);
	
	//setters
	void SetKeyboardKey(unsigned int index, bool toggle);
	void SetMouseButtons(int index, bool toggle);
	void SetMousePosition(LPARAM lparam);
};

//#pragma once
//#include <Windows.h>
//#include <windowsx.h>
//
////class InputManager
////{
////private:
//namespace Input
//{
//	enum KeyState
//	{
//		NOTPRESSED,
//		DOWN,
//		UP,
//		HELD,
//	};
//
//	//KeyState keyboard[256];
//	bool keyboard[256];
//	bool keyboardDown[256];
//	bool keyboardUp[256];
//	int mouseX, mouseY;
//	//bool rightButtonPressed, leftButtonPressed, middleButtonPressed;
//	KeyState mouseButtons[3];
//
//	bool sendEvent;
//
//	//private helper functions
//	void SendEvent();
//
//	//public:
//	void ClearKeyboard();
//	void Update();
//
//	//getters
//	bool GetKey(unsigned int button);
//	bool GetKeyDown(unsigned int button);
//	bool GetKeyUp(unsigned int button);
//	int GetMouseX();
//	int GetMouseY();
//	bool GetMouseButton(int index);
//	bool GetMouseButtonDown(int index);
//	bool GetMouseButtonUp(int index);
//
//	//I've taken out buttonUp because it's not necessarily needed, and I didn't know hot to implement without wasting
//	//processing time or memory 
//	//bool GetButtonUp(unsigned int button);
//
//	//setters
//	void SetKeyboardKey(unsigned int index, bool toggle);
//	void SetMouseButtons(int index, bool toggle);
//	void SetMousePosition(LPARAM lparam);
//	//};
//};