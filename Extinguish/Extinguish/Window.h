#pragma once
#include <windows.h>
#include <windowsx.h>
#include "Includes.h"
#include "InputManager.h"

class Window
{
private:
	WNDCLASSEX wc;
	HWND hwnd;

	//Window helper functions
	void RegisterWindowClass(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	void CreateWind(HINSTANCE hInstance);
	void ShowWind(int nCmdShow);

public:
	void Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow); //initializes window
	int Update(InputManager* input); //runs window msg loop and also handles running directx11 stuff
	const HWND GetHWND();
};