#pragma once
#include "Game.h"
#include "Movement.h"
#include "BallController.h"
#include "AI.h"

using namespace DirectX;

void Game::Init(DeviceResources* devResources, InputManager* inputManager)
{
	//initialize resource manager
	resourceManager.Init(devResources);

	if (isMultiplayer)
	{
		if (isServer)
		{
			server.init("127.0.0.1", 60000);
		}

		client.init("127.0.0.1", 60001);
	}

	currentScene = 0;

	CreateScenes(devResources, inputManager);
}

void Game::Update(float dt)
{
	//input = inputManager;

	if (isMultiplayer)
	{
		if (isServer)
		{
			int serverState = server.run();
		}

		int clientState = client.run();
	}

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

	XMFLOAT4X4 projection;
	XMMATRIX perspective = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 100.0f);
	XMStoreFloat4x4(&projection, XMMatrixTranspose(perspective));

	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, DirectX::XMMatrixIdentity());

	Scene* basic = new Scene();

	basic->Init(devResources, input);

	GameObject* camera = new GameObject();
	basic->AddGameObject(camera);
	camera->Init("Camera");
	camera->InitTransform(identity, { 0, 1.5f, -3.0f }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Camera* cameraController = new Camera();
	camera->AddComponent(cameraController);
	//cameraController->Init({ 0.0f, 0.7f, -1.5f, 0.0f }, { 0.0f, -0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);
	cameraController->Init({ 0.0f, 0.7f, 1.5f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

	GameObject* box = new GameObject();
	basic->AddGameObject(box);
	box->Init("Box");
	box->InitTransform(identity, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* boxRenderer = new Renderer();
	box->AddComponent(boxRenderer);
	boxRenderer->Init("Box", "NormalMapped", "Bind", "", "Attack", projection, &resourceManager, devResources);
	//CapsuleCollider* boxcap = new CapsuleCollider(1, { 0,0,0 }, { 0,2.7f,0 }, box, true);
	//box->AddComponent(boxcap);
	BoxCollider* boxcol = new BoxCollider(box, false, {1,1,1 }, { -1,0,-1 });
	box->AddComponent(boxcol);

	GameObject* meterbox = new GameObject();
	basic->AddGameObject(meterbox);
	meterbox->Init("MeterBox");
	meterbox->InitTransform(identity, { 3,0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* meterboxRenderer = new Renderer();
	meterbox->AddComponent(meterboxRenderer);
	meterboxRenderer->Init("MeterBox", "Static", "Static", "", "", projection, &resourceManager, devResources);
	//Movement* meterboxplayerController = new Movement();
	//meterbox->AddComponent(meterboxplayerController);
	//meterboxplayerController->Init(5.0f, 0.75f);
	//CapsuleCollider* meterboxcap = new CapsuleCollider(1, { 0,-0.5f,0 }, { 0,0.5f,0 }, meterbox, true);
	//meterbox->AddComponent(meterboxcap);
	BoxCollider* meterboxcol = new BoxCollider(meterbox, false , { 0.5f,0.5f,0.5f }, { -0.5f,-0.5f,-0.5f });
	meterbox->AddComponent(meterboxcol);
	//PlayerController* bplayerController = new PlayerController();
	//box->AddComponent(bplayerController);
	//bplayerController->Init(5.0f, 0.75f);

	GameObject* meterbox2 = new GameObject();
	basic->AddGameObject(meterbox2);
	meterbox2->Init("MeterBox2");
	meterbox2->InitTransform(identity, { 3,6, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* meterboxRenderer2 = new Renderer();
	meterbox2->AddComponent(meterboxRenderer2);
	meterboxRenderer2->Init("MeterBox", "Static", "Static", "", "", projection, &resourceManager, devResources);
	BoxCollider* meterboxcol2 = new BoxCollider(meterbox2, false, { 0.5f,0.5f,0.5f }, { -0.5f,-0.5f,-0.5f });
	meterbox2->AddComponent(meterboxcol2);

	GameObject* meterbox3 = new GameObject();
	basic->AddGameObject(meterbox3);
	meterbox3->Init("MeterBox3");
	meterbox3->InitTransform(identity, { 0,3, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* meterboxRenderer3 = new Renderer();
	meterbox3->AddComponent(meterboxRenderer3);
	meterboxRenderer3->Init("MeterBox", "Static", "Static", "", "", projection, &resourceManager, devResources);
	BoxCollider* meterboxcol3 = new BoxCollider(meterbox3, false, { 0.5f,0.5f,0.5f }, { -0.5f,-0.5f,-0.5f });
	meterbox3->AddComponent(meterboxcol3);

	GameObject* meterbox4 = new GameObject();
	basic->AddGameObject(meterbox4);
	meterbox4->Init("MeterBox4");
	meterbox4->InitTransform(identity, { 6,3, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* meterboxRenderer4 = new Renderer();
	meterbox4->AddComponent(meterboxRenderer4);
	meterboxRenderer4->Init("MeterBox", "Static", "Static", "", "", projection, &resourceManager, devResources);
	BoxCollider* meterboxcol4 = new BoxCollider(meterbox4, false, { 0.5f,0.5f,0.5f }, { -0.5f,-0.5f,-0.5f });
	meterbox4->AddComponent(meterboxcol4);

	GameObject* meterbox5 = new GameObject();
	basic->AddGameObject(meterbox5);
	meterbox5->Init("MeterBox5");
	meterbox5->InitTransform(identity, { 3,3, -3 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* meterboxRenderer5 = new Renderer();
	meterbox5->AddComponent(meterboxRenderer5);
	meterboxRenderer5->Init("MeterBox", "Static", "Static", "", "", projection, &resourceManager, devResources);
	BoxCollider* meterboxcol5 = new BoxCollider(meterbox5, false, { 0.5f,0.5f,0.5f }, { -0.5f,-0.5f,-0.5f });
	meterbox5->AddComponent(meterboxcol5);
	
	GameObject* meterbox6 = new GameObject();
	basic->AddGameObject(meterbox6);
	meterbox6->Init("MeterBox6");
	meterbox6->InitTransform(identity, { 3,3, 3 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* meterboxRenderer6 = new Renderer();
	meterbox6->AddComponent(meterboxRenderer6);
	meterboxRenderer6->Init("MeterBox", "Static", "Static", "", "", projection, &resourceManager, devResources);
	BoxCollider* meterboxcol6 = new BoxCollider(meterbox6, false, { 0.5f,0.5f,0.5f }, { -0.5f,-0.5f,-0.5f });
	meterbox6->AddComponent(meterboxcol6);

	GameObject* Ball = new GameObject();
	basic->AddGameObject(Ball);
	Ball->Init("Ball");
	Ball->InitTransform(identity, { 3,1,0 }, { 0,0,0 }, { 1,1,1 }, nullptr, nullptr, nullptr);
	Renderer* ballrenderer = new Renderer();
	Ball->AddComponent(ballrenderer);
	ballrenderer->Init("Ball", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Movement* ballMover = new Movement();
	Ball->AddComponent(ballMover);
	ballMover->Init(5.0f, 0.75f);
	SphereCollider* ballcol = new SphereCollider(0.5f, Ball, false, float3(1,1,0).normalize() * 1.5f);
	Ball->AddComponent(ballcol);

	GameObject* Ball2 = new GameObject();
	basic->AddGameObject(Ball2);
	Ball2->Init("Ball2");
	Ball2->InitTransform(identity, { 3,3,-2 }, { 0,0,0 }, { 1,1,1 }, nullptr, nullptr, nullptr);
	Renderer* ballrenderer2 = new Renderer();
	Ball2->AddComponent(ballrenderer2);
	ballrenderer2->Init("Ball", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Movement* ballMover2 = new Movement();
	Ball2->AddComponent(ballMover2);
	ballMover2->Init(5.0f, 0.75f);
	CapsuleCollider* ballcol2 = new CapsuleCollider(0.5f, { 0,0,0 }, { 0,3,0 }, Ball2, false);
	Ball2->AddComponent(ballcol2);

	GameObject* Ball3 = new GameObject();
	basic->AddGameObject(Ball3);
	Ball3->Init("Ball3");
	Ball3->InitTransform(identity, { 3,3,2 }, { 0,0,0 }, { 1,1,1 }, nullptr, nullptr, nullptr);
	Renderer* ballrenderer3 = new Renderer();
	Ball3->AddComponent(ballrenderer3);
	ballrenderer3->Init("Ball", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Movement* ballMover3 = new Movement();
	//Ball3->AddComponent(ballMover3);
	//ballMover3->Init(5.0f, 0.75f);
	CapsuleCollider* ballcol3 = new CapsuleCollider(0.5f, { 0,0,0 }, { 0,1,0 }, Ball3, false);
	Ball3->AddComponent(ballcol3);

	GameObject* bear = new GameObject();
	basic->AddGameObject(bear);
	bear->Init("Bear");
	bear->InitTransform(identity, { 5, 0, 0 }, { 0, -180, 0 }, { 0.01f, 0.01f, 0.01f }, nullptr, nullptr, nullptr);
	Renderer* bearRenderer = new Renderer();
	bear->AddComponent(bearRenderer);
	bearRenderer->Init("Teddy", "NormalMapped", "Bind", "", "Idle", projection, &resourceManager, devResources);
	//PlayerController* playerController = new PlayerController();
	//bear->AddComponent(playerController);
	//playerController->Init();
	Movement* bearMover = new Movement();
	bear->AddComponent(bearMover);
	bearMover->Init(100.0f, 0.75f);
	BoxCollider* bearcol = new BoxCollider(bear, true, { 1,2,1 }, { -1,0,-1 });
	bear->AddComponent(bearcol);
	

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


	GameObject* gameBall = new GameObject();
	basic->AddGameObject(gameBall);
	GameObject* crosse = new GameObject();
	basic->AddGameObject(crosse);

	gameBall->Init("GameBall");
	gameBall->InitTransform(identity, { 0, 0.5f, -0.3f }, { 0, 0, 0 }, { 0.1f, 0.1f, 0.1f }, crosse->GetTransform(), nullptr, nullptr);
	Renderer* gameBallRenderer = new Renderer();
	gameBall->AddComponent(gameBallRenderer);
	gameBallRenderer->Init("Ball", "Static", "Static", "", "", projection, &resourceManager, devResources);
	SphereCollider* gameBallCollider = new SphereCollider(0.05f, gameBall, false);
	gameBall->AddComponent(gameBallCollider);
	gameBallCollider->Init(gameBall);
	BallController* ballController = new BallController(gameBall);
	gameBall->AddComponent(ballController);
	ballController->Init();

	crosse->Init("Crosse");
	crosse->InitTransform(identity, { 0, 0.20f, 0.9f }, { 1 * XM_PI / 2, 0, 0}, { 1, 1, 1 }, camera->GetTransform(), nullptr, nullptr);
	//crosse->InitTransform(identity, { 0.5f, 0.15f, 0.9f }, { 0, 1 * XM_PI, -0.25f * XM_PI }, { 0.001f, 0.001f, 0.001f }, camera->GetTransform(), nullptr, nullptr);
	SphereCollider* crosseNetCollider = new SphereCollider(0.7f, Ball3, true);
	crosseNetCollider->Init(crosse);
	Renderer* crosseRenderer = new Renderer();
	crosse->AddComponent(crosseRenderer);
	crosseRenderer->Init("Crosse", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Crosse* crosseController = new Crosse();
	crosse->AddComponent(crosseController);
	crosseController->Init();
	
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