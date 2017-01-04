#pragma once
#include "Game.h"

void Game::Init(DeviceResources const * devResources)
{
	CreateScenes(devResources);
}

void Game::Update(InputManager& inputManager)
{
	input = inputManager;

	for (int i = 0; i < scenes.size(); ++i)
	{
		scenes[i].Update(input);
	}
}

void Game::Render()
{
	for (int i = 0; i < scenes.size(); ++i)
	{
		scenes[i].Render();
	}
}

void Game::Shutdown()
{

}

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

	GameObject bear;
	AnimatedRenderNode bearRenderer;
	//bearRenderer.Init

	scenes.push_back(basic);
	scenesNamesTable.Insert("FirstLevel");
}