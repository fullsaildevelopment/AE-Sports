#include "Application.h"
#include <iostream>

void Application::Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//initialize window
	window.Init(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	//initialize device resources
	devResources.Init(window.GetHWND());

	//initialize buttons... This needs to be done before initializing games
	input = InputManager::GetSingleton();
	//input = new InputManager();
	//input->ClearKeyboard();

	//initialize sound engine
	soundEngine.InitSoundEngine();

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

	//std::cout << windowResult << std::endl;

	if (windowResult == WM_QUIT)
	{
		result = false;
	}
	else //handle game code
	{
		//game.Update(input, dt);
		input->Update();
		soundEngine.ProcessAudio();
		game.Update(dt);
		game.Render();
	}

	//present backbuffer
	devResources.Present();

	//std::cout << result << std::endl;


	return result;
}

void Application::Shutdown()
{
	//clean up any memory application dynamically allocated
	soundEngine.Terminate();
	game.Shutdown();
	devResources.Shutdown();
	delete input;
}