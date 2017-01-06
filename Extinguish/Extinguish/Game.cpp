#pragma once
#include "Game.h"

void Game::Init(DeviceResources const * devResources)
{
	//initialize resource manager
	resourceManager.Init(devResources);

	currentScene = 0;

	CreateScenes(devResources);
}

void Game::Update(InputManager& inputManager)
{
	input = inputManager;

	scenes[currentScene].Update(input);
}

void Game::Render()
{
	scenes[currentScene].Render();
}

void Game::Shutdown()
{

}

//if scenes are already all loaded, then this should be setscene instead
void Game::LoadScene(unsigned int index)
{

}

//getters//


//setters//


//private helper functions//

//this function will create a scene and add it to the scenes vector, where I can then load any scene throughout the game based on input
void Game::CreateScenes(DeviceResources const * devResources)
{
	//create menus, levels, etc.//

	//create basic level
	Scene basic;

	basic.Init(devResources);

	GameObject* bear = new GameObject;

	Renderer* bearRenderer = new Renderer();
	bearRenderer->Init("Teddy", "NormalMapped", "Bind", "", 0, &resourceManager, devResources);
	bear->AddComponent(bearRenderer);

	basic.AddGameObject(bear);

	scenes.push_back(basic);
	scenesNamesTable.Insert("FirstLevel");
}