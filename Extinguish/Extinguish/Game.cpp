#pragma once
#include "Game.h"

using namespace DirectX;

void Game::Init(DeviceResources* devResources, InputManager* inputManager)
{
	//initialize resource manager
	resourceManager.Init(devResources);

	currentScene = 0;

	CreateScenes(devResources, inputManager);
}

void Game::Update(float dt)
{
	//input = inputManager;

	//scenes[currentScene].Update(*input, dt);
	scenes[currentScene].Update(dt);
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
void Game::CreateScenes(DeviceResources* devResources, InputManager* input)
{
	//create menus, levels, etc.//

	//create basic level

	//set projection matrix
	float aspectRatio = (float)CLIENT_WIDTH / (float)CLIENT_HEIGHT;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	DirectX::XMFLOAT4X4 projection;
	XMMATRIX perspective = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 100.0f);
	XMStoreFloat4x4(&projection, XMMatrixTranspose(perspective));

	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, DirectX::XMMatrixIdentity());

	Scene basic;

	basic.Init(devResources, input);

	GameObject* box = new GameObject();
	Renderer* boxRenderer = new Renderer();
	boxRenderer->Init("Box", "NormalMapped", "Bind", "", 0, projection, &resourceManager, devResources);
	box->AddComponent(boxRenderer);
	Transform* boxTransform = new Transform();
	boxTransform->Init(identity, "Box");
	//boxTransform->SetScale({ 0.1f, 0.1f, 0.1f });
	box->AddComponent(boxTransform);
	basic.AddGameObject(box);

	GameObject* bear = new GameObject();
	Renderer* bearRenderer = new Renderer();
	bearRenderer->Init("Teddy", "NormalMapped", "Bind", "", 0, projection, &resourceManager, devResources);
	bear->AddComponent(bearRenderer);
	Transform* bearTransform = new Transform();
	bearTransform->Init(identity, "Bear");
	bearTransform->SetScale({ 0.025f, 0.025f, 0.025f });
	bearTransform->Translate({ 5, 0, 0 });
	bear->AddComponent(bearTransform); 
	basic.AddGameObject(bear);

	GameObject* plane = new GameObject();
	Renderer* planeRenderer = new Renderer();
	planeRenderer->Init("Plane", "Static", "Static", "", -1, projection, &resourceManager, devResources);
	plane->AddComponent(planeRenderer);
	Transform* planeTransform = new Transform();
	planeTransform->Init(identity, "Plane");
	plane->AddComponent(planeTransform);
	basic.AddGameObject(plane);
	//Renderer* snak = bear->GetComponent<Renderer>();

	scenes.push_back(basic);
	scenesNamesTable.Insert("FirstLevel");
}