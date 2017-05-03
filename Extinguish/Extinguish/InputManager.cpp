#include <iostream>
#include "InputManager.h"
#include "EventDispatcher.h"
#include "InputDownEvent.h"
#include "GamePadEvent.h"
#include "Game.h"
//#include "GamePad.h"

using namespace std;

InputManager *InputManager::input = 0;

InputManager::InputManager()
{
	//determine if keyboard or gamepad
	std::unique_ptr<GamePad> gamePad = std::make_unique<GamePad>();
	gamePadState = gamePad->GetState(0);

	if (gamePadState.IsConnected())
	{
		isController = true;
	}
	else
	{
		isController = false;
	}
}

InputManager::~InputManager()
{

}

void InputManager::WindowResize(uint16_t w, uint16_t h)
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

	//sendEvent = false;
	alreadySent = false;
}

//only to be used to help with event
void InputManager::Init(bool keyboard[256], bool keyboardDown[256], bool keyboardUp[256], bool mouse[3], bool mouseDown[3], bool mouseUp[3], int mouseX, int mouseY)
{
	for (int i = 0; i < 256; ++i)
	{
		this->keyboard[i] = keyboard[i];
		this->keyboardDown[i] = keyboardDown[i];
		this->keyboardUp[i] = keyboardUp[i];
	}

	for (int i = 0; i < 3; ++i)
	{
		this->mouse[i] = mouse[i];
		this->mouseDown[i] = mouseDown[i];
		this->mouseUp[i] = mouseUp[i];
	}

	this->mouseX = mouseX;
	this->mouseY = mouseY;
}

void InputManager::Update()
{
	if (isController)
	{
		std::unique_ptr<GamePad> gamePad = std::make_unique<GamePad>();
		gamePadState = gamePad->GetState(0);

		if (gamePadState.IsConnected())
		{
			GamePadEvent* gamePadEvent = new GamePadEvent();
			gamePadEvent->Init(&gamePadState, Game::GetClientID());
			EventDispatcher::GetSingleton()->DispatchTo(gamePadEvent, "Game");
			delete gamePadEvent;
		}
	}
	else
	{
		if (!alreadySent)
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

		alreadySent = false;
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
	if (toggle)
	{
		if (!keyboardDown[index] && !keyboard[index])
		{
			keyboard[index] = true;
			keyboardDown[index] = true;
			SendEvent();
			keyboardDown[index] = false;

			//cout << "Down and GetKey" << endl;
		}

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
	if (toggle)
	{
		if (!mouseDown[index] && !mouse[index])
		{
			mouseDown[index] = true;
			mouse[index] = true;
			SendEvent();
			mouseDown[index] = false;
		}
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
	EventDispatcher::GetSingleton()->DispatchTo(inputEvent, "Game");
	delete inputEvent;

	//cout << "send" << endl;

	alreadySent = true;
}