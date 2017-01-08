#include "Application.h"

void Application::Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//initialize window
	window.Init(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	//initialize device resources
	devResources.Init(window.GetHWND());

	//initalize game
	game.Init(&devResources);

	//initialize buttons
	input.ClearKeyboard();
}

bool Application::Update(float dt)
{
	bool result = true;
	int windowResult;

	//clear views
	devResources.Clear();

	//check window for messages and get input
	windowResult = window.Update(input);

	if (windowResult == WM_QUIT)
	{
		result = false;
	}
	else //handle game code
	{
		game.Update(input,dt);
		game.Render();
	}

	//present backbuffer
	devResources.Present();

	return result;
}

void Application::Shutdown()
{
	//clean up any memory application dynamically allocated
	game.Shutdown();
}