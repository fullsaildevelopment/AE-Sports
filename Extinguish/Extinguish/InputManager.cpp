#include "InputManager.h"
#include <iostream>
#include "EventDispatcher.h"
#include "InputDownEvent.h"

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

	sendEvent = false;
}

void InputManager::Update()
{
	if (mouseButtons[0] == KeyState::HELD || mouseButtons[1] == KeyState::HELD || mouseButtons[2] == KeyState::HELD)
	{
		SendEvent();
	}

	//temp fix
	bool sendEvent = false;

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
	//if (sendEvent)
	//{
	//	SendEvent();
	//	sendEvent = false;
	//}
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

	if (mouseButtons[index] == KeyState::DOWN || mouseButtons[index] == KeyState::HELD)
	{
		result = true;
	}

	return result;
}

bool InputManager::GetMouseButtonDown(int index)
{
	bool result = false;

	if (mouseButtons[index] == KeyState::DOWN)
	{
		result = true;
	}

	return result;
}

bool InputManager::GetMouseButtonUp(int index)
{
	bool result = false;

	if (mouseButtons[index] == KeyState::UP)
	{
		result = true;
	}

	return result;
}

//bool InputManager::GetButtonUp(unsigned int button)
//{
//	bool result = false;
//
//	if (keyboard[button] == KeyState::UP)
//	{
//		result = true;
//	}
//
//	return result;
//}

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
	else */if (!mouseButtons[index] && toggle)
	{
		mouseButtons[index] = KeyState::DOWN;
		SendEvent();
		mouseButtons[index] = KeyState::HELD;
	}
	else if (mouseButtons[index] && !toggle)
	{
		mouseButtons[index] = KeyState::UP;
		SendEvent();
		mouseButtons[index] = KeyState::NOTPRESSED;
	}

//cout << "MouseClick event" << endl;

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
	InputDownEvent* inputEvent = new InputDownEvent(this);
	EventDispatcher::GetSingleton()->Dispatch(inputEvent);
}

//#include "InputManager.h"
//#include <iostream>
//#include "EventDispatcher.h"
//#include "InputDownEvent.h"
//
//using namespace std;
//
//namespace Input
//{
//	void ClearKeyboard()
//	{
//		for (int i = 0; i < 256; ++i)
//		{
//			//keyboard[i] = KeyState::NOTPRESSED;
//			keyboard[i] = false;
//			keyboardDown[i] = false;
//			keyboardUp[i] = false;
//		}
//
//		sendEvent = false;
//	}
//
//	void Update()
//	{
//		if (mouseButtons[0] == KeyState::HELD || mouseButtons[1] == KeyState::HELD || mouseButtons[2] == KeyState::HELD)
//		{
//			SendEvent();
//		}
//
//		//temp fix
//		bool sendEvent = false;
//
//		//for (int i = 0; i < 256; ++i)
//		//{
//		//	if (keyboard[i])
//		//	{
//		//		sendEvent = true;
//		//		break;
//		//	}
//		//}
//
//		//if (sendEvent)
//		//{
//		//	SendEvent();
//		//}
//		//if (sendEvent)
//		//{
//		//	SendEvent();
//		//	sendEvent = false;
//		//}
//	}
//
//
//	//getters//
//
//	//will return true if down or held
//	bool GetKey(unsigned int button)
//	{
//		bool result = false;
//
//		if (keyboard[button])
//		{
//			result = true;
//		}
//
//		return result;
//	}
//
//	bool GetKeyDown(unsigned int button)
//	{
//		bool result = false;
//
//		if (keyboardDown[button])
//		{
//			result = true;
//		}
//
//		return result;
//	}
//
//	bool GetKeyUp(unsigned int button)
//	{
//		bool result = false;
//
//		//if (keyboard[button] == KeyState::UP)
//		//{
//		//	result = true;
//		//}
//
//		if (keyboardUp[button])
//		{
//			result = true;
//		}
//
//		return result;
//	}
//
//
//	int GetMouseX()
//	{
//		return mouseX;
//	}
//
//	int GetMouseY()
//	{
//		return mouseY;
//	}
//
//	bool GetMouseButton(int index)
//	{
//		bool result = false;
//
//		if (mouseButtons[index] == KeyState::DOWN || mouseButtons[index] == KeyState::HELD)
//		{
//			result = true;
//		}
//
//		return result;
//	}
//
//	bool GetMouseButtonDown(int index)
//	{
//		bool result = false;
//
//		if (mouseButtons[index] == KeyState::DOWN)
//		{
//			result = true;
//		}
//
//		return result;
//	}
//
//	bool GetMouseButtonUp(int index)
//	{
//		bool result = false;
//
//		if (mouseButtons[index] == KeyState::UP)
//		{
//			result = true;
//		}
//
//		return result;
//	}
//
//	//bool GetButtonUp(unsigned int button)
//	//{
//	//	bool result = false;
//	//
//	//	if (keyboard[button] == KeyState::UP)
//	//	{
//	//		result = true;
//	//	}
//	//
//	//	return result;
//	//}
//
//	//setters//
//
//	void SetKeyboardKey(unsigned int index, bool toggle)
//	{
//		////button is already pressed and they are still pressing
//		//if (keyboard[index] && toggle)
//		//{
//		//	keyboard[index] = KeyState::HELD;	
//		//	//cout << "Held" << endl;
//		//	SendEvent();
//		//}
//		////button isn't already pressed and they pressed it
//		//else if (!keyboard[index] && toggle)
//		//{
//		//	keyboard[index] = KeyState::DOWN;
//		//	cout << "Down" << endl;
//		//	SendEvent();
//		//}
//		//else if (!toggle)
//		//{
//		//	keyboard[index] = KeyState::UP;
//		//	SendEvent();
//		//	keyboard[index] = KeyState::NOTPRESSED;
//		//}
//
//		if (toggle)
//		{
//			if (!keyboardDown[index] && !keyboard[index])
//			{
//				keyboardDown[index] = true;
//				keyboard[index] = true;
//
//				//cout << "Down and GetKey" << endl;
//			}
//			else if (keyboardDown[index])
//			{
//				keyboardDown[index] = false;
//
//				//cout << "GetKey" << endl;
//			}
//
//			SendEvent();
//
//			//cout << "sent event" << endl;
//		}
//		else
//		{
//			keyboard[index] = false;
//			keyboardDown[index] = false;
//			keyboardUp[index] = true;
//			SendEvent();
//			keyboardUp[index] = false;
//
//			//cout << "Up" << endl;
//			//cout << "sent event" << endl;
//
//		}
//		//cout << "Keyboard event" << endl;
//
//
//	}
//
//	//-1 is left. 0 is middle. 1 is right.
//	void SetMouseButtons(int index, bool toggle)
//	{
//		//if (index == -1)
//		//{
//		//	leftButtonPressed = toggle;
//		//}
//		//else if (index)
//		//{
//		//	rightButtonPressed = toggle;
//		//}
//		//else
//		//{
//		//	middleButtonPressed = toggle;
//		//}
//
//	/*	if (mouseButtons[index] && toggle)
//		{
//			mouseButtons[index] = KeyState::HELD;
//			SendEvent();
//		}
//		else */if (!mouseButtons[index] && toggle)
//		{
//			mouseButtons[index] = KeyState::DOWN;
//			SendEvent();
//			mouseButtons[index] = KeyState::HELD;
//		}
//		else if (mouseButtons[index] && !toggle)
//		{
//			mouseButtons[index] = KeyState::UP;
//			SendEvent();
//			mouseButtons[index] = KeyState::NOTPRESSED;
//		}
//
//	//cout << "MouseClick event" << endl;
//
//	}
//
//	void SetMousePosition(LPARAM lparam)
//	{
//		mouseX = GET_X_LPARAM(lparam);
//		mouseY = GET_Y_LPARAM(lparam);
//
//		SendEvent();
//
//		//cout << "MouseMove event" << endl;
//	}
//
//	//private helper functions
//	void SendEvent()
//	{
//		InputDownEvent* inputEvent = new InputDownEvent(this);
//		EventDispatcher::GetSingleton()->Dispatch(inputEvent);
//	}
//};