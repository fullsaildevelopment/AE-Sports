#include "Application.h"

void Application::Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//initialize window
	window.Init(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	//initialize device resources
	devResources.Init(window.GetHWND());

	//initialize buttons... This needs to be done before initializing games
	input = new InputManager();
	input->ClearKeyboard();

	//initalize game
	game.Init(&devResources, input);

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
		//game.Update(input, dt);
		game.Update(dt);
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
	devResources.Shutdown();
	delete input;
}