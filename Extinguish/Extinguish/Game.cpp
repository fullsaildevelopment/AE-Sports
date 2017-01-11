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
	scenes[currentScene]->Update(dt);
}

void Game::Render()
{
	scenes[currentScene]->Render();
}

void Game::Shutdown()
{
	for (int i = 0; i < scenes.size(); ++i)
	{
		scenes[i]->Shutdown();
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
//add the component before you initialize, so you can cache from the game object (this works because init sets component's game object pointer)
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

	Scene* basic = new Scene();

	basic->Init(devResources, input);

	GameObject* camera = new GameObject();
	basic->AddGameObject(camera);
	camera->Init("Camera");
	camera->InitTransform(identity, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Camera* cameraController = new Camera();
	camera->AddComponent(cameraController);
	//cameraController->Init({ 0.0f, 0.7f, -1.5f, 0.0f }, { 0.0f, -0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);
	cameraController->Init({ 0.0f, 0.7f, -1.5f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

	GameObject* box = new GameObject();
	basic->AddGameObject(box);
	box->Init("Box");
	box->InitTransform(identity, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* boxRenderer = new Renderer();
	box->AddComponent(boxRenderer);
	boxRenderer->Init("Box", "NormalMapped", "Bind", "", "Attack", projection, &resourceManager, devResources);
	//PlayerController* bplayerController = new PlayerController();
	//box->AddComponent(bplayerController);
	//bplayerController->Init(5.0f, 0.75f);

	GameObject* bear = new GameObject();
	basic->AddGameObject(bear);
	bear->Init("Bear");
	bear->InitTransform(identity, { 5, 0, 0 }, { 0, -180, 0 }, { 0.025f, 0.025f, 0.025f }, nullptr, nullptr, nullptr);
	Renderer* bearRenderer = new Renderer();
	bear->AddComponent(bearRenderer);
	bearRenderer->Init("Teddy", "NormalMapped", "Bind", "", "Idle", projection, &resourceManager, devResources);
	PlayerController* playerController = new PlayerController();
	bear->AddComponent(playerController);
	playerController->Init(50.0f, 0.75f);
	//Camera* cameraController = new Camera();
	//bear->AddComponent(cameraController);
	//cameraController->Init({ 0.0f, 0.7f, -1.5f, 0.0f }, { 0.0f, -0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

	//Transform* bearHead = new Transform();
	//bearHead->SetLocal(bearRenderer->GetBlender()->GetSkeleton().GetBones()[5].world);
	//bearHead->Init(bearRenderer->GetBlender()->GetSkeleton().GetBones()[5].world, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, nullptr, nullptr, nullptr);

	//bearHead->Init(

	GameObject* plane = new GameObject();
	basic->AddGameObject(plane);
	plane->Init("Plane");
	plane->InitTransform(identity, { 0, 0, 0 }, { 0, 0, 0 }, { 10, 0.1f, 10 }, nullptr, nullptr, nullptr);
	Renderer* planeRenderer = new Renderer();
	plane->AddComponent(planeRenderer);
	planeRenderer->Init("Plane", "Static", "Static", "", "", projection, &resourceManager, devResources);

	GameObject* crosse = new GameObject();
	basic->AddGameObject(crosse);
	crosse->Init("Crosse");
	crosse->InitTransform(identity, { -2, 0, 0 }, { 0, -180, -45 }, { 0.1f, 0.1f, 0.1f }, bear->GetTransform(), nullptr, nullptr);
	Renderer* crosseRenderer = new Renderer();
	crosse->AddComponent(crosseRenderer);
	crosseRenderer->Init("Crosse", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Crosse* crosseController = new Crosse();
	crosse->AddComponent(crosseController);
	
	GameObject* mage = new GameObject();
	basic->AddGameObject(mage);
	mage->Init("Mage");
	mage->InitTransform(identity, { -5, 0, 0 }, { 0, 0, 0}, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* mageRenderer = new Renderer();
	mage->AddComponent(mageRenderer);
	mageRenderer->Init("Mage", "NormalMapped", "Bind", "", "Idle", projection, &resourceManager, devResources);
	//PlayerController* bplayerController = new PlayerController();
	//mage->AddComponent(bplayerController);
	//bplayerController->Init(5.0f, 0.75f);

	scenes.push_back(basic);
	scenesNamesTable.Insert("FirstLevel");
}