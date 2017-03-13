#include <iostream>
#include "Application.h"

void Application::Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//initialize window
	window.Init(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	//initialize device resources
	devResources.Init(window.GetHWND());

	//initialize buttons... This needs to be done before initializing games
	input = InputManager::GetSingleton();
	//input = new InputManager();
	input->ClearKeyboard();

	//initalize game
	game.Init(&devResources, input);

}

bool Application::Update(float _dt)
{
	bool result = true;
	int windowResult;

	//clear views
	devResources.Clear();

	//check window for messages and get input
	windowResult = window.Update(input);

	if (ResourceManager::GetSingleton()->IsServer())
		Game::dt = _dt;

	//std::cout << windowResult << std::endl;
	if (windowResult == WM_QUIT)
	{
		result = false;
	}
	else //handle game code
	{
		//game.Update(input, dt);
		//TODO: make input inside of game, and only allow server to update it
		input->Update();
		//soundEngine.ProcessAudio();
		int gResult = game.Update();
		if (gResult == 0)
			result = false;
		game.Render();
	}

	//present backbuffer
	devResources.Present();

	//std::cout << result << std::endl;


	return result;
}

bool Application::FixedUpdate(float _dt)
{
	game.FixedUpdate();
	return true;
}

void Application::Shutdown()
{
	//clean up any memory application dynamically allocated
	//soundEngine.Terminate();
	game.Shutdown();
	devResources.Shutdown();
	delete input;
}