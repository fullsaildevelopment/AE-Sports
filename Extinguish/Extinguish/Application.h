#pragma once
#include "Window.h"
#include "Scene.h"
#include "DeviceResources.h"
#include "ResourceManager.h"
#include "Game.h"
#include "InputManager.h"
#include "SoundEngine.h"

class Application
{
private:
	//object members
	Window window;
	DeviceResources devResources;
	Game game;
	InputManager* input;
	SoundEngine soundEngine;

public:
	void Init(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	bool Update(float dt);
	void Shutdown();
};