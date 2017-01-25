#pragma once
#include "Game.h"
#include "Movement.h"
#include "BallController.h"
#include "AI.h"
#include "EventDispatcher.h"

using namespace DirectX;
using namespace std;

//initialize static member
int Game::clientID = 1;

void Game::Init(DeviceResources* devResources, InputManager* inputManager)
{
	//register to event dispatcher
	EventDispatcher::GetSingleton()->RegisterHandler(this);

	//initialize resource manager
	resourceManager.Init(devResources);

	if (isMultiplayer)
	{
		if (server.init("127.0.0.1", 60000) == 1)
		{
			isMultiplayer = true;
			isServer = true;

			client.init("127.0.0.1", 60001);
		}
		else
		{
			isServer = false;
			client.init("127.0.0.1", 60001);
		}
	}

	currentScene = 0;

	CreateScenes(devResources, inputManager);

	//initialize sound engine
	std::vector<unsigned int> ids;
	std::vector<std::string> names;

	std::vector<GameObject*>* gameObjects = scenes[currentScene]->GetGameObjects();

	ids.resize(scenes[currentScene]->GetNumObjects());
	names.resize(scenes[currentScene]->GetNumObjects());

	for (int i = 0; i < gameObjects->size(); ++i)
	{
		GameObject* gameObject = (*gameObjects)[i];
		
		ids[i] = i;
		names[i] = gameObject->GetName();
	}
	gameStates.resize(scenes[currentScene]->GetNumObjects());
	soundEngine.InitSoundEngine(ids, names);

	if (isServer)
	{
		server.setObjCount(scenes[currentScene]->GetNumObjects());
	}
}

void Game::Update(float dt)
{
	//input = inputManager;

	if (isMultiplayer)
	{
		//set client id
		Game::clientID = client.getID();

		// get current game states
		std::vector<GameObject*>* gameObjects = scenes[currentScene]->GetGameObjects();

		/*gameStates.resize(scenes[currentScene]->GetNumObjects());*/

		for (int i = 0; i < gameObjects->size(); ++i)
		{
			GameState* state = new GameState();
			GameObject* gameObject = (*gameObjects)[i];
			float3 position = gameObject->GetTransform()->GetPosition();
			float3 rotation = gameObject->GetTransform()->GetRotation();
			state->position = { position.x, position.y, position.z };
			state->rotation = { rotation.x, rotation.y, rotation.z };

			gameStates[i] = state;
		}

		// if server, set game states
		if (isServer)
		{
			server.SetGameStates(gameStates);
		}

		if (client.getID() > 0)
		{
			// get camera position
			client.setLocation(gameStates[0]->position);
			client.setRotation(gameStates[0]->rotation);
			// send to server
			client.sendPacket();
		}

		if (isServer)
		{
			int serverState = server.run();
			if (serverState == 2)
			{
				gameStates = server.getStates();
			}
		}

		int clientState = client.run();

		// if client gets server's game states, get the state's location from the client
		// so that it can be included in update
		if (clientState == 2 && client.getID() > 0)
		{
			unsigned int numobjs = (unsigned int)scenes[currentScene]->GetNumObjects();

			int id = client.getID();
			for (unsigned int i = 0; i < numobjs; ++i)
			{
				if (i != 0 && i != id)
				{
					GameObject* gameObject = (*gameObjects)[i];
					XMFLOAT3 position, rotation;
					position = client.getLocation(i);
					rotation = client.getRotation(i);
					gameObject->GetTransform()->SetPosition({ position.x, position.y, position.z });
					gameObject->GetTransform()->SetRotation({ rotation.x, rotation.y, rotation.z });
					//gameObject->GetTransform()->SetLocal(client.getLocation(i));
					//gameStates[i]->world = client.getLocation(i);
				}
			}
		}
	}

	//scenes[currentScene].Update(*input, dt);
	scenes[currentScene]->Update(dt);

	soundEngine.ProcessAudio();
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
		delete scenes[i];
	}

	for (int i = 0; i < gameStates.size(); ++i)
	{
		delete gameStates[i];
	}

	soundEngine.Terminate();
}

//misc

//if scenes are already all loaded, then this should be setscene instead
void Game::LoadScene(unsigned int index)
{

}

void Game::HandleEvent(Event* e)
{
	//filter throw events to find right one
	InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

	if (inputDownEvent)
	{
		//if it's the server, but the messenger is a client, dispatch a message from server to all components to handle input... or messenger is server, but not marked as one
		if ((isServer && inputDownEvent->GetID() != 1 && !inputDownEvent->IsServer()) || ((!inputDownEvent->IsServer() && inputDownEvent->GetID() == 1)))
		{
			//inputDownEvent->SetID(clientID);
			inputDownEvent->SetIsServer(true);
			EventDispatcher::GetSingleton()->Dispatch(inputDownEvent);
		}
		else if (inputDownEvent->GetID() > 1) //if not server, give server your input to handle it
		{
			client.sendInput(inputDownEvent);
		}
	}
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
	meterboxRenderer2->Init("Axis", "Static", "Static", "", "", projection, &resourceManager, devResources);
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
	meterbox6->InitTransform(identity, { 0,0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* meterboxRenderer6 = new Renderer();
	meterbox6->AddComponent(meterboxRenderer6);
	meterboxRenderer6->Init("MeterBox", "Static", "Static", "", "", projection, &resourceManager, devResources);
	BoxCollider* meterboxcol6 = new BoxCollider(meterbox6, false, { 300,0.5f,300 }, { -300,-0.5f,-300 });
	meterbox6->AddComponent(meterboxcol6);

	meterbox6->SetTag("Team1");
	AI *mbox6AI = new AI(meterbox6);
	meterbox6->AddComponent(mbox6AI);



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
	SphereCollider* ballcol2 = new SphereCollider(0.5f, Ball2, false);
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
	SphereCollider* ballcol3 = new SphereCollider(0.5f, Ball3, false);
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
	bearMover->Init(1.0f, 0.75f);
	BoxCollider* bearcol = new BoxCollider(bear, false, { 1,2,1 }, { -1,0,-1 });
	bear->AddComponent(bearcol);
	
	bear->SetTag("Team1");
	AI *bearAI = new AI(bear);
	bear->AddComponent(bearAI);

	

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
	plane->InitTransform(identity, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* planeRenderer = new Renderer();
	plane->AddComponent(planeRenderer);
	planeRenderer->Init("Plane", "Static", "Static", "", "", projection, &resourceManager, devResources);


	GameObject* gameBall = new GameObject();
	basic->AddGameObject(gameBall);
	GameObject* crosse = new GameObject();
	basic->AddGameObject(crosse);

	gameBall->Init("GameBall");
	gameBall->InitTransform(identity, { 0, 0, 0 }, { 0, 0, 0 }, { 0.2f, 0.2f, 0.2f }, crosse->GetTransform(), nullptr, nullptr);
	//gameBall->InitTransform(identity, { -5, 0.5f, -2.5f }, { 0, 0, 0 }, {0.2f, 0.2f, 0.2f }, crosse->GetTransform(), nullptr, nullptr);
	Renderer* gameBallRenderer = new Renderer();
	gameBall->AddComponent(gameBallRenderer);
	gameBallRenderer->Init("Ball", "Static", "Static", "", "", projection, &resourceManager, devResources);
	SphereCollider* gameBallCollider = new SphereCollider(0.125f, gameBall, false);
	gameBall->AddComponent(gameBallCollider);
	BallController* ballController = new BallController(gameBall);
	gameBall->AddComponent(ballController);
	ballController->Init();
	ballController->SetHolder(crosse);

	gameBall->SetTag("Ball");
	mbox6AI->Init();
	bearAI->Init();


	crosse->Init("Crosse");
	crosse->InitTransform(identity, { 0, 0.20f, 0.9f }, { 0, 0, 0}, { 1, 1, 1 }, camera->GetTransform(), nullptr, nullptr);
	//crosse->InitTransform(identity, { 0.5f, 0.15f, 0.9f }, { 0, 1 * XM_PI, -0.25f * XM_PI }, { 0.001f, 0.001f, 0.001f }, camera->GetTransform(), nullptr, nullptr);
	SphereCollider* crosseNetCollider = new SphereCollider(0.25f, crosse, true);
	crosse->AddComponent(crosseNetCollider);
	Renderer* crosseRenderer = new Renderer();
	crosse->AddComponent(crosseRenderer);
	crosseRenderer->Init("Crosse", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Crosse* crosseController = new Crosse();
	crosse->AddComponent(crosseController);
	crosseController->Init();

	GameObject* crosse2 = new GameObject();
	basic->AddGameObject(crosse2);
	crosse2->Init("Crosse2");
	crosse2->InitTransform(identity, { 0, 0.20f, 0.9f }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	//crosse->InitTransform(identity, { 0.5f, 0.15f, 0.9f }, { 0, 1 * XM_PI, -0.25f * XM_PI }, { 0.001f, 0.001f, 0.001f }, camera->GetTransform(), nullptr, nullptr);
	SphereCollider* crosseNetCollider2 = new SphereCollider(0.25f, crosse2, true);
	crosse2->AddComponent(crosseNetCollider2);
	Renderer* crosseRenderer2 = new Renderer();
	crosse2->AddComponent(crosseRenderer);
	crosseRenderer2->Init("Crosse", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Crosse* crosseController2 = new Crosse();
	crosse2->AddComponent(crosseController2);
	crosseController2->Init();
	
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

//getters
int Game::GetClientID()
{
	return clientID;
}