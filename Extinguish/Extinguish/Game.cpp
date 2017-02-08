#pragma once
#include "Game.h"
#include "Movement.h"
#include "BallController.h"
#include "AI.h"
#include "EventDispatcher.h"
#include "Physics.h"
#include "AnimatorController.h"
#include "State.h"
#include "Transition.h"
#include "Trigger.h"
#include "Transform.h"
#include "Crosse.h"
#include "PlayerController.h"
#include "Camera.h"
#include "BoxCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "SoundEngine.h"
#include "EventHandler.h"
#include "HexagonCollider.h"
#include "FloorController.h"
#include "Button.h"
#include "UIRenderer.h"
#include "ScoreEvent.h"
#include "Goal.h"
#include "LoadSceneEvent.h"

using namespace DirectX;
using namespace std;

//initialize static member
int Game::clientID = 1;

ClientWrapper Game::client;
ServerWrapper Game::server;
unsigned int Game::currentScene;

void Game::Init(DeviceResources* devResources, InputManager* inputManager)
{
	//cache
	soundEngine = SoundEngine::GetSingleton();
	resourceManager = ResourceManager::GetSingleton();

	//register to event dispatcher
	EventDispatcher::GetSingleton()->RegisterHandler(this, "Game");

	//initialize resource manager
	resourceManager->Init(devResources);

	/*if (isMultiplayer)
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

	}*/

	currentScene = 0;

	//create scenes
	CreateScenes(devResources, inputManager);

	//set up server stuff
	std::vector<GameObject*>* gameObjects = scenes[currentScene]->GetGameObjects();

	gameStates.resize(scenes[currentScene]->GetNumObjects());

	if (gameStates.size() > 1)
		float temp = 0.1f;
	for (int i = 0; i < gameObjects->size(); ++i)
	{
		GameState* state = new GameState();
		gameStates[i] = state;
	}

	/*if (isServer)
	{
		server.setObjCount(scenes[currentScene]->GetNumObjects());
	}*/

	//init sound engine
	std::vector<unsigned int> ids;
	std::vector<std::string> names;

	ids.resize(scenes[currentScene]->GetNumObjects());
	names.resize(scenes[currentScene]->GetNumObjects());

	for (int i = 0; i < gameObjects->size(); ++i)
	{
		GameObject* gameObject = (*gameObjects)[i];

		ids[i] = i;
		names[i] = gameObject->GetName();
	}

	soundEngine->InitSoundEngine(ids, names);
}

void Game::WindowResize(uint16_t w, uint16_t h)
{
	//set projection matrix
	float aspectRatio = (float)w / (float)h;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	XMFLOAT4X4 projection;
	XMMATRIX perspective = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 500.0f);
	XMStoreFloat4x4(&projection, XMMatrixTranspose(perspective));

	vector<GameObject*> go = *scenes[currentScene]->GetGameObjects();
	int size = go.size();
	for (int i = 0; i < size; ++i)
	{
		go[i]->GetComponent<Renderer>()->SetProjection(projection);
	}
}

void Game::Update(float dt)
{
	if (ResourceManager::GetSingleton()->IsMultiplayer())
	{
		if (server.getObjCount() == 0)
			server.setObjCount(scenes[currentScene]->GetNumObjects());

		//set client id
		Game::clientID = client.getID();

		// if server, set game states
		if (ResourceManager::GetSingleton()->IsServer())
		{
			UpdateServerStates();
		}

		//run server
		if (ResourceManager::GetSingleton()->IsServer())
		{
			int serverState = server.run();
		}

		//run client
		int clientState = client.run();

		// if client gets server's game states, get the state's location from the client
		// so that it can be included in update
		if ((clientState == 2 || clientState == 4) && client.getID() > 0)
		{
			UpdateClientObjects();

			if (clientState == 4)
			{
				Team1Score = client.getScoreA();
				Team2Score = client.getScoreB();
				UpdateUI();
			}
		}
	}

	//update current scene
	scenes[currentScene]->Update(dt);

	//render audio
	soundEngine->ProcessAudio();
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

	soundEngine->Terminate();
	resourceManager->Shutdown();
}

//misc

//void Game::LoadScene(unsigned int index)
//{
//
//}

void Game::HandleEvent(Event* e)
{
	//filter throw events to find right one
	InputDownEvent* inputDownEvent = dynamic_cast<InputDownEvent*>(e);

	if (inputDownEvent)
	{
		//if the game is the server, but the messenger is a client, dispatch a message from server to all components to handle input... or if messenger is server, but not marked as one
		if ((ResourceManager::GetSingleton()->IsServer() && inputDownEvent->GetID() != 1 && !inputDownEvent->IsServer()) || ((!inputDownEvent->IsServer() && inputDownEvent->GetID() == 1)))
		{
			//inputDownEvent->SetID(clientID);
			inputDownEvent->SetIsServer(true);
			EventDispatcher::GetSingleton()->Dispatch(inputDownEvent);
		}
		else if (inputDownEvent->GetID() > 1 && !inputDownEvent->IsServer()) //if not server, give server your input to handle it
		{
			client.sendInput(inputDownEvent);
		}

		return;
	}

	ScoreEvent* SEvent = dynamic_cast<ScoreEvent*>(e);
	if (SEvent)
	{
		switch (SEvent->GetTeam())
		{
		case 0:
			++Team1Score;
			break;
		case 1:
			++Team2Score;
			break;
		default:
			break;
		}
		if (ResourceManager::GetSingleton()->IsMultiplayer() && ResourceManager::GetSingleton()->IsServer())
		{
			server.setScores(Team1Score, Team2Score);
			server.sendGameState();
		}
		UpdateUI();
		//Reset Game

		return;
	}

	LoadSceneEvent* loadSceneEvent = dynamic_cast<LoadSceneEvent*>(e);

	if (loadSceneEvent)
	{
		LoadScene(loadSceneEvent->GetName());

		return;
	}
}

//getters//
int Game::GetClientID()
{
	return clientID;
}

//setters//
float3* CreateFloor(float d, int r, int c, float3 p)
{
	float3* poses = new float3[r * c];
	float s = d * 0.5f;
	float h = (float)sqrt(3.0)*s;
	for (int i = 0; i < r; ++i)
	{
		for (int j = 0; j < c; ++j)
		{
			if (i % 2 == 0)
			{
				poses[i * c + j].x = p.x + (j * h);
				poses[i * c + j].z = p.z + ((d + s) * (i * 0.5f));
			}
			else
			{
				poses[i * c + j].x = p.x + (j * h) + (0.5f*h);
				poses[i * c + j].z = p.z + ((d + s) * (i * 0.5f));
			}
			poses[i * c + j].y = p.y;
		}
	}
	return poses;
}

//private helper functions//

//this function will create a scene and add it to the scenes vector, where I can then load any scene throughout the game based on input
//add the component before you initialize, so you can cache from the game object (this works because init sets component's game object pointer)
void Game::CreateScenes(DeviceResources* devResources, InputManager* input)
{

	//set projection matrix
	float aspectRatio = (float)CLIENT_WIDTH / (float)CLIENT_HEIGHT;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	XMFLOAT4X4 projection;
	XMMATRIX perspective = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 500.0f);
	XMStoreFloat4x4(&projection, XMMatrixTranspose(perspective));

	XMFLOAT4X4 identity;
	XMStoreFloat4x4(&identity, DirectX::XMMatrixIdentity());

	//create menus, levels, etc.//

	Scene* menu = new Scene();
	// ask tom
	GameObject* camera = new GameObject();
	menu->AddGameObject(camera);
	camera->Init("Camera1");
	camera->InitTransform(identity, { 0, 0, -1.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Camera* cameraController = new Camera();
	camera->AddComponent(cameraController);
	cameraController->Init({ 0.0f, 0.7f, 1.5f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

	menu->Init(devResources, input);

	menu->set2DRenderTarget(devResources->GetRenderTarget());

	CreateMenu(devResources, menu);

	scenes.push_back(menu);
	scenesNamesTable.Insert("Menu");

	//create basic level


	Scene* basic = new Scene();

	basic->Init(devResources, input);

	basic->set2DRenderTarget(devResources->GetRenderTarget());

	//used for hexagon floor
	int row = 80; // * 2 = z
	int col = 80; // * 2 = x

	//deleted inside a different class
	unsigned int* colors = new unsigned int[row * col];

	GameObject* gameBall = new GameObject();
	basic->AddGameObject(gameBall);
	gameBall->Init("GameBall");
	gameBall->InitTransform(identity, { -7, 10, -20.5f }, { 0, 0, 0 }, { 0.2f, 0.2f, 0.2f }, nullptr, nullptr, nullptr);
	Renderer* gameBallRenderer = new Renderer();
	gameBall->AddComponent(gameBallRenderer);
	gameBallRenderer->Init("Ball", "Static", "Static", "", "", projection, devResources);
	SphereCollider* gameBallCollider = new SphereCollider(0.125f, gameBall, false);
	gameBall->AddSphereCollider(gameBallCollider);
	Physics* physics = new Physics();
	gameBall->AddComponent(physics);
	physics->Init();
	BallController* ballController = new BallController(gameBall);
	gameBall->AddComponent(ballController);
	ballController->Init();
	gameBall->SetTag("Ball");

	for (int i = 1; i <= 8; ++i)
	{
		string playerName = "Mage";
		playerName += to_string(i);

		GameObject* mage1 = new GameObject();
		basic->AddGameObject(mage1);
		mage1->Init(playerName);

		int tempCol = col;

		if (i > 4)
		{
			tempCol = -tempCol;
		}

		mage1->InitTransform(identity, { (float)tempCol, 0.0f, -12.0f + i * 4.0f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);

		Renderer* mageRenderer1 = new Renderer();
		mage1->AddComponent(mageRenderer1);
		mageRenderer1->Init("Mage", "NormalMapped", "Bind", "", "Idle", projection, devResources);
		Movement* mageMover = new Movement();
		mage1->AddComponent(mageMover);
		mageMover->Init(5.0f, 0.75f);
		PlayerController* bplayerController = new PlayerController();
		mage1->AddComponent(bplayerController);
		bplayerController->Init();
		CapsuleCollider* mageCollider1 = new CapsuleCollider(0.5f, { 0, 0, 0 }, { 0, 5, 0 }, mage1, false);
		mage1->AddCapsuleCollider(mageCollider1);
		mageCollider1->Init(mage1);

		AnimatorController* mageAnim1 = new AnimatorController();
		mage1->AddComponent(mageAnim1);
		mageAnim1->Init("Mage", 0, "Idle"); //init animator
		State* mageIdle = new State();
		mageAnim1->AddState(mageIdle);
		mageIdle->Init(mageAnim1, mageAnim1->GetBlender()->GetAnimationSet()->GetAnimation("Idle"), true, 0.5f, "Idle");
		State* mageRun = new State();
		mageAnim1->AddState(mageRun);
		mageRun->Init(mageAnim1, mageAnim1->GetBlender()->GetAnimationSet()->GetAnimation("Run"), true, 0.75f, "Run");
		State* mageStumble = new State();
		mageAnim1->AddState(mageStumble);
		mageStumble->Init(mageAnim1, mageAnim1->GetBlender()->GetAnimationSet()->GetAnimation("StumbleBackwards3"), false, 0.75f, "Stumble");
		mageAnim1->UpdateCurAnimatorsLoopAndSpeed(); //needs to be done after states are created and added
		Param::Trigger* runTrigger = new Param::Trigger();
		runTrigger->Init("Run", false); //must init trigger before adding to animator
		mageAnim1->AddParameter(runTrigger);
		Param::Trigger* stumbleTrigger = new Param::Trigger();
		stumbleTrigger->Init("Stumble", false);
		mageAnim1->AddParameter(stumbleTrigger);
		Param::Trigger* idleTrigger = new Param::Trigger();
		idleTrigger->Init("Idle", false);
		mageAnim1->AddParameter(idleTrigger);
		Transition* idleToRun = new Transition();
		mageIdle->AddTransition(idleToRun);
		idleToRun->Init(mageIdle, mageRun, -1, 5.0f);
		idleToRun->AddCondition(runTrigger);
		Transition* runToIdle = new Transition();
		mageRun->AddTransition(runToIdle);
		runToIdle->Init(mageRun, mageIdle, -1, 0.1f);
		runToIdle->AddCondition(idleTrigger);
		Transition* idleToStumble = new Transition();
		mageIdle->AddTransition(idleToStumble);
		idleToStumble->Init(mageIdle, mageStumble, -1, 0.01f);
		idleToStumble->AddCondition(stumbleTrigger);
		Transition* stumbleToIdle = new Transition();
		mageStumble->AddTransition(stumbleToIdle);
		stumbleToIdle->Init(mageStumble, mageIdle, 0, 0.01f); //exit time of 0 will make transition happen right when cur animation is done

		string cameraName = "Camera";
		cameraName += to_string(i);

		GameObject* camera1 = new GameObject();
		basic->AddGameObject(camera1);
		camera1->Init(cameraName);
		camera1->InitTransform(identity, { 0, 0, -1.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage1->GetTransform(), nullptr, nullptr);
		//camera1->InitTransform(identity, { 0, 0, -15.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage1->GetTransform(), nullptr, nullptr);
		Camera* cameraController1 = new Camera();
		camera1->AddComponent(cameraController1);
		cameraController1->Init({ 0.0f, 0.7f, 1.5f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

		string crosseName = "Crosse";
		crosseName += to_string(i);

		GameObject* crosse = new GameObject();
		basic->AddGameObject(crosse);
		crosse->Init(crosseName);
		crosse->InitTransform(identity, { 0, 5.4f, -1.7f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage1->GetTransform(), nullptr, nullptr);
		SphereCollider* crosseNetCollider = new SphereCollider(0.75f, crosse, true);
		crosse->AddSphereCollider(crosseNetCollider);
		Renderer* crosseRenderer = new Renderer();
		crosse->AddComponent(crosseRenderer);
		crosseRenderer->Init("Crosse", "Static", "Static", "", "", projection, devResources);
		Crosse* crosseController = new Crosse();
		crosse->AddComponent(crosseController);
		crosseController->Init();
	}

	GameObject* goal = new GameObject();
	basic->AddGameObject(goal);
	goal->Init("Goal");
	goal->InitTransform(identity, { -7, 0, (float)-row }, { 0,0,0 }, { 1,1,1 }, nullptr, nullptr, nullptr);
	Renderer* GoalRenderer = new Renderer();
	goal->AddComponent(GoalRenderer);
	GoalRenderer->Init("Goal", "Static", "Static", "", "", projection, devResources);
	BoxCollider* Goal1col = new BoxCollider(goal, true, { 3,20,3 }, { -3,0,0 });
	goal->AddBoxCollider(Goal1col);
	Goal* g1 = new Goal(goal);
	goal->AddComponent(g1);

	GameObject* goal2 = new GameObject();
	basic->AddGameObject(goal2);
	goal2->Init("Goal2");
	goal2->InitTransform(identity, { -7, 0, (float)row - 38 }, { 0, 3.14159f, 0 }, { 1,1,1 }, nullptr, nullptr, nullptr);
	Renderer* GoalRenderer2 = new Renderer();
	goal2->AddComponent(GoalRenderer2);
	GoalRenderer2->Init("Goal", "Static", "Static", "", "", projection, devResources);
	BoxCollider* Goal2col = new BoxCollider(goal2, true, { 3,20,3 }, { -3,0,0 });
	goal2->AddBoxCollider(Goal2col);
	Goal* g2 = new Goal(goal2);
	goal2->AddComponent(g2);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GameObject* meterbox6 = new GameObject();
	basic->AddGameObject(meterbox6);
	meterbox6->Init("MeterBox6");
	meterbox6->InitTransform(identity, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* meterboxRenderer6 = new Renderer();
	meterbox6->AddComponent(meterboxRenderer6);
	meterboxRenderer6->Init("MeterBox", "Static", "Static", "", "", projection, devResources);
	BoxCollider* meterboxcol6 = new BoxCollider(meterbox6, false, { 300,0.5f,300 }, { -300,-0.5f,-300 });
	meterbox6->AddBoxCollider(meterboxcol6);
	float3* floor = CreateFloor(2.0f, row, col, float3((float)-row, -10, (float)-col));

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	GameObject* HexFloor = new GameObject();
	basic->AddGameObject(HexFloor);
	HexFloor->Init("HexFloor");
	HexFloor->InitTransform(identity, { 0,0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* HexFloorRenderer = new Renderer();
	HexFloor->AddComponent(HexFloorRenderer);
	HexFloorRenderer->Init(row * col, floor, colors, "Hexagon", "InstStatic", "InstancedStatic", "", "", projection, devResources);
	HexagonCollider* HexFLoorCol = new HexagonCollider(row, col, floor, 10, 2, HexFloor);
	HexFloor->AddComponent(HexFLoorCol);
	FloorController* fcon = new FloorController(floor, row, col, 10, colors);
	HexFloor->AddComponent(fcon);

	GameObject* Hex = new GameObject();
	basic->AddGameObject(Hex);
	Hex->Init("Team2");
	Hex->InitTransform(identity, { (float)-col - 3, 0, -4 }, { 0,0,0 }, { 1,1,1 }, nullptr, nullptr, nullptr);
	Renderer* ballrenderer3 = new Renderer();
	Hex->AddComponent(ballrenderer3);
	ballrenderer3->Init("Hexagon", "Static", "Static", "", "", projection, devResources);
	Movement* ballMover3 = new Movement();
	HexagonCollider* HexPillar = new HexagonCollider(Hex, 2, 10);
	Hex->AddComponent(HexPillar);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// so that we keep the chunk of 3d object creation and 2d object creation separate
	CreateUI(devResources, basic);

	scenes.push_back(basic);
	scenesNamesTable.Insert("FirstLevel");
}

void Game::CreateUI(DeviceResources * devResources, Scene * basic)
{
	GameObject * testScore = new GameObject();
	basic->AddUIObject(testScore);
	testScore->Init("gameScore");
	Button * theSButton = new Button(true, true, L"0 : 0", (unsigned int)strlen("0 : 0"), 500.0f, 100.0f, devResources, 0);
	theSButton->SetGameObject(testScore);
	theSButton->showFPS(false);
	theSButton->setOrigin(250.0f, 30.0f);
	theSButton->setPositionMultipliers(0.5f, 0.0f);
	testScore->AddComponent(theSButton);
	UIRenderer * scoreRender = new UIRenderer();
	scoreRender->Init(true, 35.0f, devResources, theSButton);
	scoreRender->DecodeBitmap(L"../Assets/UI/trapezoid.png");
	testScore->AddComponent(scoreRender);
	scoreRender->MakeRTSize();
	theSButton->MakeRect();
	scoreRender->InitMetrics();

	GameObject * debugUI = new GameObject();
	basic->AddUIObject(debugUI);
	debugUI->Init("debugUI");
	Button * theButton = new Button(true, true, L"Titans with Sticks", (unsigned int)strlen("Titans with Sticks"), 350.0f, 100.0f, devResources, 0);
	theButton->SetGameObject(debugUI);
	theButton->showFPS(true);
	theButton->setOrigin(0.0f, 30.0f);
	debugUI->AddComponent(theButton);
	UIRenderer * buttonRender = new UIRenderer();
	buttonRender->Init(true, 30.0f, devResources, theButton);
	debugUI->AddComponent(buttonRender);
}

void Game::CreateMenu(DeviceResources * devResources, Scene * scene)
{
	// title
	GameObject * title = new GameObject();
	scene->AddUIObject(title);
	title->Init("title");
	Button * tButton = new Button(true, false, L"", 0, 400.0f, 400.0f, devResources, 0);
	tButton->SetGameObject(title);
	tButton->showFPS(false);
	tButton->setOrigin(0.0f, 0.0f);
	tButton->setPositionMultipliers(0.5f, 0.25f);
	title->AddComponent(tButton);
	UIRenderer * tRender = new UIRenderer();
	tRender->Init(true, 35.0f, devResources, tButton);
	tRender->DecodeBitmap(L"../Assets/UI/newTitle.png");
	title->AddComponent(tRender);
	tRender->MakeRTSize();
	tButton->MakeRect();

	// solo button
	GameObject * soloPlayer = new GameObject();
	scene->AddUIObject(soloPlayer);
	soloPlayer->Init("soloPlayer");
	Button * sButton = new Button(true, true, L"Play Game", (unsigned int)strlen("Play Game"), 300.0f, 60.0f, devResources, 3);
	sButton->SetGameObject(soloPlayer);
	sButton->showFPS(false);
	sButton->setOrigin(350.0f, 425.0f);
	sButton->setPositionMultipliers(0.5f, 0.55f);
	soloPlayer->AddComponent(sButton);
	UIRenderer * sRender = new UIRenderer();
	sRender->Init(true, 25.0f, devResources, sButton);
	sRender->DecodeBitmap(L"../Assets/UI/button2.png");
	sRender->DecodeBitmap(L"../Assets/UI/button3.png");
	soloPlayer->AddComponent(sRender);
	sRender->MakeRTSize();
	sButton->MakeRect();
	sButton->MakeHandler();
	sRender->InitMetrics();

	// host button
	GameObject * multiPlayer = new GameObject();
	scene->AddUIObject(multiPlayer);
	multiPlayer->Init("multiHost");
	Button * mButton = new Button(true, true, L"Host", (unsigned int)strlen("Host"), 145.0f, 60.0f, devResources, 1);
	mButton->SetGameObject(multiPlayer);
	mButton->showFPS(false);
	mButton->setOrigin(350.0f, 505.0f);
	mButton->setPositionMultipliers(0.425f, 0.65f);
	multiPlayer->AddComponent(mButton);
	UIRenderer * mRender = new UIRenderer();
	mRender->Init(true, 25.0f, devResources, mButton);
	mRender->DecodeBitmap(L"../Assets/UI/button4.png");
	mRender->DecodeBitmap(L"../Assets/UI/button5.png");
	multiPlayer->AddComponent(mRender);
	mRender->MakeRTSize();
	mButton->MakeRect();
	mButton->MakeHandler();
	mRender->InitMetrics();

	// join button
	GameObject * multiPlayer2 = new GameObject();
	scene->AddUIObject(multiPlayer2);
	multiPlayer->Init("multiJoin");
	Button * mButton2 = new Button(true, true, L"Join", (unsigned int)strlen("Join"), 145.0f, 60.0f, devResources, 2);
	mButton2->SetGameObject(multiPlayer2);
	mButton2->showFPS(false);
	mButton2->setOrigin(500.0f, 505.0f);
	mButton2->setPositionMultipliers(0.575f, 0.65f);
	multiPlayer2->AddComponent(mButton2);
	UIRenderer * mRender2 = new UIRenderer();
	mRender2->Init(true, 25.0f, devResources, mButton2);
	mRender2->DecodeBitmap(L"../Assets/UI/button4.png");
	mRender2->DecodeBitmap(L"../Assets/UI/button5.png");
	multiPlayer2->AddComponent(mRender2);
	mRender2->MakeRTSize();
	mButton2->MakeRect();
	mButton2->MakeHandler();
	mRender2->InitMetrics();
	
	// credits

	GameObject * credits = new GameObject();
	scene->AddUIObject(credits);
	credits->Init("credits");
	Button * cButton = new Button(true, true, L"Credits", (unsigned int)strlen("Credits"), 300.0f, 60.0f, devResources, 4);
	cButton->SetGameObject(credits);
	cButton->showFPS(false);
	cButton->setOrigin(350.0f, 585.0f);
	cButton->setPositionMultipliers(0.5f, 0.75f);
	credits->AddComponent(cButton);
	UIRenderer * cRender = new UIRenderer();
	cRender->Init(true, 25.0f, devResources, cButton);
	cRender->DecodeBitmap(L"../Assets/UI/button2.png");
	cRender->DecodeBitmap(L"../Assets/UI/button3.png");
	credits->AddComponent(cRender);
	cRender->MakeRTSize();
	cButton->MakeRect();
	cButton->MakeHandler();
	cRender->InitMetrics();

	// exit

	GameObject * exit = new GameObject();
	scene->AddUIObject(exit);
	exit->Init("exit");
	Button * eButton = new Button(true, true, L"Exit", (unsigned int)strlen("Exit"), 300.0f, 60.0f, devResources, 5);
	eButton->SetGameObject(exit);
	eButton->showFPS(false);
	eButton->setOrigin(350.0f, 665.0f);
	eButton->setPositionMultipliers(0.5f, 0.85f);
	exit->AddComponent(eButton);
	UIRenderer * eRender = new UIRenderer();
	eRender->Init(true, 25.0f, devResources, eButton);
	eRender->DecodeBitmap(L"../Assets/UI/button2.png");
	eRender->DecodeBitmap(L"../Assets/UI/button3.png");
	exit->AddComponent(eRender);
	eRender->MakeRTSize();
	eButton->MakeRect();
	eButton->MakeHandler();
	eRender->InitMetrics();


	// create lobby
	CreateLobby(devResources, scene);
}

void Game::CreateLobby(DeviceResources * devResources, Scene * scene)
{
	// host button


	// join button


	// input name


	// lobby screen -- show current players

}


void Game::UpdateServerStates()
{
	// get current game states
	std::vector<GameObject*>* gameObjects = scenes[currentScene]->GetGameObjects();

	for (int i = 0; i < gameStates.size(); ++i)
	{
		GameState* state = gameStates[i];
		GameObject* gameObject = (*gameObjects)[i];

		float3 position = gameObject->GetTransform()->GetPosition();
		float3 rotation = gameObject->GetTransform()->GetRotation();
		state->position = { position.x, position.y, position.z };
		state->rotation = { rotation.x, rotation.y, rotation.z };

		int parentIndex = -1;
		Transform* parent = gameObject->GetTransform()->GetParent();

		if (parent)
		{
			for (parentIndex = 0; parentIndex < gameObjects->size(); ++parentIndex)
			{
				GameObject* gameObject2 = (*gameObjects)[parentIndex];

				if (parent == gameObject2->GetTransform())
				{
					break;
				}
			}
		}

		state->parentIndex = parentIndex;

		int animIndex = -1;

		AnimatorController* animator = gameObject->GetComponent<AnimatorController>();

		if (animator)
		{
			animIndex = animator->GetCurrentStateIndex();
		}

		if (animIndex == 1)
		{
			int breakpoint = 0;
			breakpoint += 69;
		}

		state->animationIndex = animIndex;
	}

	server.SetGameStates(gameStates);
}

void Game::UpdateClientObjects()
{
	// get current game states
	std::vector<GameObject*>* gameObjects = scenes[currentScene]->GetGameObjects();

	unsigned int numobjs = (unsigned int)scenes[currentScene]->GetNumObjects();

	int id = client.getID();

	if (id != 1)
	{
		//remove children of every object
		for (unsigned int i = 0; i < numobjs; ++i)
		{
			GameObject* gameObject = (*gameObjects)[i];
			gameObject->GetTransform()->RemoveChildren();
		}

		for (unsigned int i = 0; i < numobjs; ++i)
		{
			//if (i != 0 && i != id)
			//if (i != id)
			{
				GameObject* gameObject = (*gameObjects)[i];
				XMFLOAT3 position, rotation;
				position = client.getLocation(i);
				rotation = client.getRotation(i);

				if (i == 20)
				{
					int breakpoint = 0;
					breakpoint = 5;
				}

				gameObject->GetTransform()->SetPosition({ position.x, position.y, position.z });
				gameObject->GetTransform()->SetRotation({ rotation.x, rotation.y, rotation.z });

				INT8 parentIndex = client.GetParentIndex(i);
				if (parentIndex >= 0)
				{
					gameObject->GetTransform()->SetParent((*gameObjects)[parentIndex]->GetTransform());
				}

				INT8 animIndex = client.GetAnimationIndex(i);

				if (animIndex >= 0)
				{
					//Renderer* renderer = gameObject->GetComponent<Renderer>();

					//if (renderer)
					//{
					//	Blender* blender = renderer->GetBlender();

					//	if (blender)
					//	{
					//		/*								if (animIndex == 1)
					//		{
					//		int breakpoint = 0;
					//		breakpoint += 69;
					//		}*/

					//		//if (!blender->GetNextInterpolator()->HasAnimation())
					//		if ((blender->GetCurInterpolator()->GetAnimation()->GetAnimationName() != blender->GetAnimationSet()->GetAnimation(animIndex)->GetAnimationName()))
					//		{
					//			BlendInfo info;
					//			info.totalBlendTime = 0.01f;

					//			renderer->SetNextAnimation(animIndex);
					//			renderer->SetBlendInfo(info);

					//			cout << "Change in animation" << endl;
					//		}
					//	}
					//}

					AnimatorController* animator = gameObject->GetComponent<AnimatorController>();

					if (animator)
					{
						if (animator->GetCurrentStateIndex() != animIndex)
						{
							animator->SetCurrentState(animIndex);


							cout << animIndex << endl;
						}
					}
				}
			}
		}
	}
}

void Game::UpdateUI()
{
	wstring newScore = to_wstring(Team1Score) + L" : " + to_wstring(Team2Score);

	GameObject * score = scenes[currentScene]->GetUIByName("gameScore");
	Button * button = score->GetComponent<Button>();
	button->setText(newScore);
}

//if scenes are already all loaded, then this should be setscene instead
void Game::LoadScene(std::string name)
{
	//change currentSceneIndex
	int index = scenesNamesTable.GetKey(name);

	if (index != -1)
	{
		currentScene = index;
	}

	//resize gamestates
	gameStates.resize(scenes[currentScene]->GetNumObjects());
	for (int i = 0; i < gameStates.size(); ++i)
	{
		GameState* state = new GameState();
		gameStates[i] = state;
	}
}