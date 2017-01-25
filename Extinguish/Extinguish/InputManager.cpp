#include <iostream>
#include "InputManager.h"
#include "EventDispatcher.h"
#include "InputDownEvent.h"
#include "Game.h"

using namespace std;

InputManager *InputManager::input = 0;

InputManager::~InputManager()
{

}

void InputManager::ClearKeyboard()
{
	for (int i = 0; i < 256; ++i)
	{
		//keyboard[i] = KeyState::NOTPRESSED;
		keyboard[i] = false;
		keyboardDown[i] = false;
		keyboardUp[i] = false;
	}

	for (int i = 0; i < 3; ++i)
	{
		mouse[i] = false;
		mouseDown[i] = false;
		mouseUp[i] = false;
	}

	sendEvent = false;
}

void InputManager::Update()
{
	bool sendEvent = false;

	for (int i = 0; i < 3; ++i)
	{
		if (mouse[i])
		{
			sendEvent = true;
			break;
		}
	}

	for (int i = 0; i < 256; ++i)
	{
		if (keyboard[i])
		{
			sendEvent = true;
			break;
		}
	}

	if (sendEvent)
	{
		SendEvent();
	}
}

void InputManager::Shutdown()
{
	delete input;
	input = nullptr;
}

//getters//

InputManager* InputManager::GetSingleton()
{
	if (!input)
	{
		input = new InputManager();
	}
	
	return input;
}

//will return true if down or held
bool InputManager::GetKey(unsigned int button)
{
	bool result = false;

	if (keyboard[button])
	{
		result = true;
	}

	return result; 
}

bool InputManager::GetKeyDown(unsigned int button)
{
	bool result = false;

	if (keyboardDown[button])
	{
		result = true;
	}

	return result;
}

bool InputManager::GetKeyUp(unsigned int button)
{
	bool result = false;

	//if (keyboard[button] == KeyState::UP)
	//{
	//	result = true;
	//}

	if (keyboardUp[button])
	{
		result = true;
	}

	return result;
}


int InputManager::GetMouseX()
{
	return mouseX;
}

int InputManager::GetMouseY()
{
	return mouseY;
}

bool InputManager::GetMouseButton(int index)
{
	bool result = false;

	if (mouse[index])
	{
		result = true;
	}

	return result;
}

bool InputManager::GetMouseButtonDown(int index)
{
	bool result = false;

	if (mouseDown[index])
	{
		result = true;
	}

	return result;
}

bool InputManager::GetMouseButtonUp(int index)
{
	bool result = false;

	if (mouseUp[index])
	{
		result = true;
	}

	return result;
}

bool* InputManager::GetKeyboard()
{
	return keyboard;
}

bool* InputManager::GetKeyboardDown()
{
	return keyboardDown;
}

bool* InputManager::GetKeyboardUp()
{
	return keyboardUp;
}

bool* InputManager::GetMouse()
{
	return mouse;
}

bool* InputManager::GetMouseDown()
{
	return mouseDown;
}

bool* InputManager::GetMouseUp()
{
	return mouseUp;
}

//setters//

void InputManager::SetKeyboardKey(unsigned int index, bool toggle)
{
	////button is already pressed and they are still pressing
	//if (keyboard[index] && toggle)
	//{
	//	keyboard[index] = KeyState::HELD;	
	//	//cout << "Held" << endl;
	//	SendEvent();
	//}
	////button isn't already pressed and they pressed it
	//else if (!keyboard[index] && toggle)
	//{
	//	keyboard[index] = KeyState::DOWN;
	//	cout << "Down" << endl;
	//	SendEvent();
	//}
	//else if (!toggle)
	//{
	//	keyboard[index] = KeyState::UP;
	//	SendEvent();
	//	keyboard[index] = KeyState::NOTPRESSED;
	//}

	if (toggle)
	{
		if (!keyboardDown[index] && !keyboard[index])
		{
			keyboardDown[index] = true;
			keyboard[index] = true;

			//cout << "Down and GetKey" << endl;
		}
		else if (keyboardDown[index])
		{
			keyboardDown[index] = false;

			//cout << "GetKey" << endl;
		}

		SendEvent();

		//cout << "sent event" << endl;
	}
	else
	{
		keyboard[index] = false;
		keyboardDown[index] = false;
		keyboardUp[index] = true;
		SendEvent();
		keyboardUp[index] = false;

		//cout << "Up" << endl;
		//cout << "sent event" << endl;

	}
	//cout << "Keyboard event" << endl;


}

//-1 is left. 0 is middle. 1 is right.
void InputManager::SetMouseButtons(int index, bool toggle)
{
	//if (index == -1)
	//{
	//	leftButtonPressed = toggle;
	//}
	//else if (index)
	//{
	//	rightButtonPressed = toggle;
	//}
	//else
	//{
	//	middleButtonPressed = toggle;
	//}

/*	if (mouseButtons[index] && toggle)
	{
		mouseButtons[index] = KeyState::HELD;
		SendEvent();
	}
	else *///if (!mouseButtons[index] && toggle)
	/*{
		mouseButtons[index] = KeyState::DOWN;
		SendEvent();
		mouseButtons[index] = KeyState::HELD;
	}
	else if (mouseButtons[index] && !toggle)
	{
		mouseButtons[index] = KeyState::UP;
		SendEvent();
		mouseButtons[index] = KeyState::NOTPRESSED;
	}*/

	if (toggle)
	{
		if (!mouseDown[index] && !mouse[index])
		{
			mouseDown[index] = true;
			mouse[index] = true;
		}
		else if (keyboardDown[index])
		{
			mouseDown[index] = false;
		}

		SendEvent();
	}
	else
	{
		mouse[index] = false;
		mouseDown[index] = false;
		mouseUp[index] = true;
		SendEvent();
		mouseUp[index] = false;
	}
}

void InputManager::SetMousePosition(LPARAM lparam)
{
	mouseX = GET_X_LPARAM(lparam);
	mouseY = GET_Y_LPARAM(lparam);

	SendEvent();

	//cout << "MouseMove event" << endl;
}

//private helper functions
void InputManager::SendEvent()
{
	//this will send input to game and anything else that handles input
	InputDownEvent* inputEvent = new InputDownEvent(this, Game::GetClientID(), false);
	EventDispatcher::GetSingleton()->Dispatch(inputEvent);
	delete inputEvent;
}