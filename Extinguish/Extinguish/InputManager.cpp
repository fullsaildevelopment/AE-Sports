#include "InputManager.h"
#include <iostream>

void InputManager::ClearKeyboard()
{
	for (int i = 0; i < 256; ++i)
	{
		keyboard[i] = KeyState::NOTPRESSED;
	}
}

//getters//

//will return true if down or held
bool InputManager::GetKey(unsigned int button)
{
	bool result = false;

	if (keyboard[button] == KeyState::DOWN || keyboard[button] == KeyState::HELD)
	{
		result = true;
	}

	return result; 
}

bool InputManager::GetKeyDown(unsigned int button)
{
	bool result = false;

	if (keyboard[button] == KeyState::DOWN)
	{
		result = true;
		std::cout << "down" << std::endl;
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
	bool result = rightButtonPressed;

	if (index == -1)
	{
		result = leftButtonPressed;
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
	//button is already pressed and they are still pressing
	if (keyboard[index] && toggle)
	{
		keyboard[index] = KeyState::HELD;
	}
	//button is already pressed and they let go
	//else if (keyboard[index] && !toggle)
	//{
	//	keyboard[index] = KeyState::UP;
	//}
	//button isn't already pressed and they pressed it
	else if (!keyboard[index] && toggle)
	{
		keyboard[index] = KeyState::DOWN;
	}
	else if (!toggle)
	{
		keyboard[index] = KeyState::NOTPRESSED;
	}

	//std::cout << keyboard[index] << " " << toggle << std::endl;
}

//-1 is left. 0 is middle. 1 is right.
void InputManager::SetMouseButtons(int index, bool toggle)
{
	if (index == -1)
	{
		leftButtonPressed = toggle;
	}
	else if (index)
	{
		rightButtonPressed = toggle;
	}
}

void InputManager::SetMousePosition(LPARAM lparam)
{
	mouseX = GET_X_LPARAM(lparam);
	mouseY = GET_Y_LPARAM(lparam);
}