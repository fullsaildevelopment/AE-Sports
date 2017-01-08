#pragma once
#include "Game.h"

void Game::Init(DeviceResources* devResources)
{
	//initialize resource manager
	resourceManager.Init(devResources);

	currentScene = 0;

	CreateScenes(devResources);
}

void Game::Update(InputManager& inputManager, float dt)
{
	input = inputManager;

	scenes[currentScene].Update(input, dt);
}

void Game::Render()
{
	scenes[currentScene].Render();
}

void Game::Shutdown()
{
	for (int i = 0; i < scenes.size(); ++i)
	{
		scenes[i].Shutdown();
	}
}

//if scenes are already all loaded, then this should be setscene instead
void Game::LoadScene(unsigned int index)
{

}

//getters//


//setters//


//private helper functions//

//this function will create a scene and add it to the scenes vector, where I can then load any scene throughout the game based on input
void Game::CreateScenes(DeviceResources* devResources)
{
	//create menus, levels, etc.//

	//create basic level
	Scene basic;

	basic.Init(devResources);

	GameObject* box = new GameObject();
	Renderer* boxRenderer = new Renderer();
	boxRenderer->Init("Box", "NormalMapped", "Bind", "", 0, &resourceManager, devResources);
	boxRenderer->SetModel(DirectX::XMMatrixIdentity());
	box->AddComponent(boxRenderer);
	basic.AddGameObject(box);

	GameObject* bear = new GameObject;
	Renderer* bearRenderer = new Renderer();
	bearRenderer->Init("Teddy", "NormalMapped", "Bind", "", 0, &resourceManager, devResources);
	//bearRenderer->Init("Teddy", "NormalMapped", "Bind", "", 0, &resourceManager, devResources);
	//bearRenderer->SetModel(DirectX::XMMatrixIdentity());
	bear->AddComponent(bearRenderer);
	//basic.AddGameObject(bear);


	//Renderer* snak = bear->GetComponent<Renderer>();

	scenes.push_back(basic);
	scenesNamesTable.Insert("FirstLevel");
}