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
#include "WindowResizeEvent.h"
#include "LoadSceneEvent.h"
#include "SoundEvent.h"
#include "CoughtEvent.h"
#include "GamePadEvent.h"

using namespace DirectX;
using namespace std;

//initialize static member
int Game::clientID = 1;

ClientWrapper Game::client;
ServerWrapper Game::server;
unsigned int Game::currentScene;
int Game::returnResult = 1;
int Game::objID = 1;
Game::PLAYER_TEAM Game::team = PLAYER_TEAM::TEAM_A;
UINT8 Game::objIDs[10];

Game::~Game()
{
	
}

void Game::Init(DeviceResources* _devResources, InputManager* inputManager)
{
	//cache
	soundEngine = SoundEngine::GetSingleton();
	resourceManager = ResourceManager::GetSingleton();

	devResources = _devResources;

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
	CreateScenes(inputManager);

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
	int loadSceneIndex = scenesNamesTable.GetKey("FirstLevel");

	ids.resize(scenes[loadSceneIndex]->GetNumObjects());
	names.resize(scenes[loadSceneIndex]->GetNumObjects());
	gameObjects = scenes[loadSceneIndex]->GetGameObjects();

	//int id = 0;
	for (int i = 0; i < gameObjects->size(); ++i)
	{
		//if (i != GetPlayerObjectID())
		{
			GameObject* gameObject = (*gameObjects)[i];

			ids[i] = i;
			names[i] = (gameObject->GetName());
		}
		//else
		//{
		//	id -= 1;
		//}
	}

	soundEngine->InitSoundEngine(ids, names);

	SoundEngine::GetSingleton()->PostEvent(AK::EVENTS::PLAY_BACKBOARD_BOUNCE_SONG, 1);
}

void Game::WindowResize(uint16_t w, uint16_t h)
{
	//set projection matrix
	devResources->ResizeWindow(w, h);

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
	Renderer* R;
	int size = (int)go.size();
	for (int i = 0; i < size; ++i)
	{
		R = go[i]->GetComponent<Renderer>();
		if (R)
			R->SetProjection(projection);
	}
}

int Game::Update(float dt)
{
	if (ResourceManager::GetSingleton()->IsMultiplayer())
	{
		if (currentScene >= 2) {
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

			if (clientState == 0)
			{
				returnResult = 0;
			}

			// if client gets server's game states, get the state's location from the client
			// so that it can be included in update
			if ((clientState == 2 || clientState == 4) && client.getID() > 0)
			{
				UpdateClientObjects();

				if (clientState == 4)
				{
					Team1Score = client.getScoreA();
					Team2Score = client.getScoreB();
					UpdateScoreUI();
				}
			}
		}
		else
		{
			int result = UpdateLobby();
			if (result == 0)
			{
				LoadScene("Menu");
				ResourceManager::GetSingleton()->SetMultiplayer(false);
				ResourceManager::GetSingleton()->SetServer(true);
			}
		}
	}

	//update current scene
	scenes[currentScene]->Update(dt);

	//render audio
	if (clientID == 0)
	{
		clientID = 1;
	}

	vector<GameObject*>* objects = scenes[scenesNamesTable.GetKey("FirstLevel")]->GetGameObjects();
	vector<XMFLOAT3> objectsPos, forwards;
	objectsPos.resize(objects->size());
	forwards.resize(objects->size());

	for (int i = 0; i < objects->size(); ++i)
	{
		//if (i != GetPlayerObjectID())
		{
			//XMFLOAT3 objectPos;

			objectsPos[i].x = (*objects)[i]->GetTransform()->GetPosition().x;
			objectsPos[i].y = (*objects)[i]->GetTransform()->GetPosition().y;
			objectsPos[i].z = (*objects)[i]->GetTransform()->GetPosition().z;

			//objectsPos.push_back(objectPos);

			//XMFLOAT3 forward;
			forwards[i] = (*objects)[i]->GetTransform()->GetForward();
			//forwards.push_back(forward);
		}
	}
	int index = (clientID - 1) * 3 + 2;

	soundEngine->UpdateListener(objectsPos[(clientID - 1) * 3 + 2], forwards[(clientID - 1) * 3 + 2]);
	soundEngine->UpdatePositions(objectsPos, forwards);
	soundEngine->ProcessAudio();
	
	return returnResult;
}

void Game::FixedUpdate(float dt)
{
	scenes[currentScene]->FixedUpdate(dt);
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
		//(currentScene < 2)
		if ((ResourceManager::GetSingleton()->IsServer() && inputDownEvent->GetID() != 1 && !inputDownEvent->IsServer()) || ((!inputDownEvent->IsServer() && inputDownEvent->GetID() == 1)))
		{
			//inputDownEvent->SetID(clientID);
			inputDownEvent->SetIsServer(true);
			EventDispatcher::GetSingleton()->Dispatch(inputDownEvent);
		}
		//TODO: need to make this work for in-game menu if we ever add
		else if (scenesNamesTable.GetKey("Lobby") == currentScene && !inputDownEvent->IsServer()) //if it's a HUD scene, let it handle the input itself
		{
			inputDownEvent->SetIsServer(true);
			EventDispatcher::GetSingleton()->Dispatch(inputDownEvent);
			//cout << "HUD stuff" << endl;
		}
		else if (inputDownEvent->GetID() > 1 && !inputDownEvent->IsServer()) //if not server, give server your input to handle it
		{
			client.sendInput(inputDownEvent);
		}

		if (ResourceManager::GetSingleton()->IsServer() && currentScene == 2)
		{
			InputManager* input = inputDownEvent->GetInput();
			if (input->GetKeyDown('	'))
			{
				GameObject * pauseResume = scenes[currentScene]->GetUIByName("pauseResume");
				GameObject * pauseExit = scenes[currentScene]->GetUIByName("pauseExit");
				GameObject * pauseMenu = scenes[currentScene]->GetUIByName("pauseMenu");
				Button * resumeButton = pauseResume->GetComponent<Button>();
				Button * exitButton = pauseExit->GetComponent<Button>();
				Button * menuButton = pauseMenu->GetComponent<Button>();
				resumeButton->SetActive(true);
				exitButton->SetActive(true);
				menuButton->SetActive(true);
			}
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
		UpdateScoreUI();
		//Reset Game

		return;
	}
	else
	{
		WindowResizeEvent* wre = dynamic_cast<WindowResizeEvent*>(e);
		if (wre)
		{
			WindowResize(wre->w, wre->h);
		}
		else
		{
			LoadSceneEvent* loadSceneEvent = dynamic_cast<LoadSceneEvent*>(e);

			if (loadSceneEvent)
			{
				if (loadSceneEvent->GetName() == "Menu" && currentScene == 2)
				{
					CreateGameWrapper();
				}
				LoadScene(loadSceneEvent->GetName());

				return;
			}
		}
	}

	SoundEvent* soundEvent = dynamic_cast<SoundEvent*>(e);

	if (soundEvent)
	{
		soundEngine->PostEvent(soundEvent->GetSoundID(), soundEvent->GetObjectID());
		gameStates[soundEvent->GetObjectID()]->soundID = soundEvent->GetSoundID();
		gameStates[soundEvent->GetObjectID()]->hasSound = true;

		//cout << "sound event received by game" << endl;

		return;
	}

	CoughtEvent* coughtEvent = dynamic_cast<CoughtEvent*>(e);
	if (coughtEvent)
	{
		gameStates[coughtEvent->id]->hasBall = coughtEvent->holding;
		return;
	}

	GamePadEvent* gamePadEvent = dynamic_cast<GamePadEvent*>(e);

	if (gamePadEvent)
	{
		//if the game is the server, but the messenger is a client, dispatch a message from server to all components to handle input... or if messenger is server, but not marked as one
		//(currentScene < 2)
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
}

//getters//
int Game::GetClientID()
{
	return clientID;
}

int Game::GetPlayerObjectID()
{
	return (clientID - 1) * 3 + 2;
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
void Game::CreateScenes(InputManager* input)
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
	if (!DEBUG_GRAPHICS) {
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

		CreateMenu(menu);

		scenes.push_back(menu);
		scenesNamesTable.Insert("Menu");


		// create lobby
		Scene* lobby = new Scene();
		// ask tom
		GameObject* lcamera = new GameObject();
		lobby->AddGameObject(lcamera);
		lcamera->Init("Camera1");
		lcamera->InitTransform(identity, { 0, 0, -1.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
		Camera* cameraControllerL = new Camera();
		lcamera->AddComponent(cameraControllerL);
		cameraControllerL->Init({ 0.0f, 0.7f, 1.5f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

		lobby->Init(devResources, input);

		lobby->set2DRenderTarget(devResources->GetRenderTarget());
		CreateLobby(lobby);
		scenes.push_back(lobby);
		scenesNamesTable.Insert("Lobby");
	}
	//create basic level


	Scene* basic = new Scene();

	basic->Init(devResources, input);

	basic->set2DRenderTarget(devResources->GetRenderTarget());

	CreateGame(basic, identity, projection);


	scenes.push_back(basic);
	scenesNamesTable.Insert("FirstLevel");
}

void Game::CreateGameWrapper()
{
	for (unsigned int i = 0; i < scenes.size(); ++i)
	{
		unsigned int obj = scenes[i]->GetNumObjects();

		for (unsigned int j = 0; j < obj; ++j)
		{
			Transform * trans = scenes[i]->GetGameObjects(j)->GetTransform();
			if (trans)
				trans->Reset();
		}
	}

	Team1Score = Team2Score = 0;
	UpdateScoreUI();
	Button * time = scenes[2]->GetUIByName("gameScoreBase")->GetComponent<Button>();
	time->resetTime();
}

void Game::CreateGame(Scene * basic, XMFLOAT4X4 identity, XMFLOAT4X4 projection)
{

	//used for hexagon floor
	int row = 80; // * 2 = z
	int col = 80; // * 2 = x

	//deleted inside a different class
	unsigned int* colors = new unsigned int[row * col];

	GameObject* gameBall = new GameObject();
	basic->AddGameObject(gameBall);
	gameBall->Init("GameBall");
	gameBall->InitTransform(identity, { -7, 15, -20.5f }, { 0, 0, 0 }, { 0.2f, 0.2f, 0.2f }, nullptr, nullptr, nullptr);
	Renderer* gameBallRenderer = new Renderer();
	gameBall->AddComponent(gameBallRenderer);
	gameBallRenderer->Init("Ball", "Ball", "Static", "", "", projection, devResources);
	SphereCollider* gameBallCollider = new SphereCollider(0.125f, gameBall, false);
	gameBall->AddSphereCollider(gameBallCollider);
	Physics* physics = new Physics();
	gameBall->AddComponent(physics);
	physics->Init();
	BallController* ballController = new BallController(gameBall);
	gameBall->AddComponent(ballController);
	ballController->Init();
	gameBall->SetTag("Ball");


	GameObject* goal = new GameObject();
	GameObject* goal2 = new GameObject();
	goal->SetTag("Goal1");
	goal2->SetTag("Goal2");

	//vector<AI*> ai;

	for (int i = 1; i <= 8; ++i)
	{
		string playerName = "Mage";
		playerName += to_string(i);

		GameObject* crosse = new GameObject();
		basic->AddGameObject(crosse);
		objIDs[i - 1] = (UINT8)basic->GetNumObjects();
		GameObject* mage1 = new GameObject();
		basic->AddGameObject(mage1);
		mage1->Init(playerName);
		mage1->SetTag("Team1");

		int tempCol = col - 25;

		if (i > 4)
		{
			tempCol = -col + 14;
			mage1->SetTag("Team2");
		}

		mage1->InitTransform(identity, { (float)tempCol, 0.0f, -12.0f + i * 4.0f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);

		Renderer* mageRenderer1 = new Renderer();
		mage1->AddComponent(mageRenderer1);
		mageRenderer1->Init("Mage", "NormalMapped", "Bind", "", "Idle", projection, devResources);

		if (i <= 4)
			mageRenderer1->SetTeamColor({ 1,0,0,0 });
		else
			mageRenderer1->SetTeamColor({ 0,0,1,0 });

		Movement* mageMover = new Movement();
		mage1->AddComponent(mageMover);
		mageMover->Init(300, 0.75f);
		PlayerController* bplayerController = new PlayerController();
		mage1->AddComponent(bplayerController);
		bplayerController->Init();
		CapsuleCollider* mageCollider1 = new CapsuleCollider(0.6f, { 0, 0.6f, 0 }, { 0, 5, 0 }, mage1, false);
		mage1->AddCapsuleCollider(mageCollider1);
		mageCollider1->Init(mage1);
		Physics* physics = new Physics(0.01f, 10.0f, 0.07f, 20, -14.8f);
		mage1->AddComponent(physics);
		physics->Init();

		AnimatorController* mageAnim1 = new AnimatorController();
		mage1->AddComponent(mageAnim1);
		mageAnim1->Init("Mage", 0, "Idle"); //init animator
		State* mageIdle = new State();
		mageIdle->Init(mageAnim1, mageAnim1->GetBlender()->GetAnimationSet()->GetAnimation("Idle"), true, 0.5f, "Idle");
		mageAnim1->AddState(mageIdle);
		State* mageRun = new State();
		mageRun->Init(mageAnim1, mageAnim1->GetBlender()->GetAnimationSet()->GetAnimation("Run"), true, 0.75f, "Run");
		mageAnim1->AddState(mageRun);
		State* mageStumble = new State();
		mageStumble->Init(mageAnim1, mageAnim1->GetBlender()->GetAnimationSet()->GetAnimation("StumbleBackwards3"), false, 0.75f, "Stumble");
		mageAnim1->AddState(mageStumble);
		State* mageJump = new State();
		mageJump->Init(mageAnim1, mageAnim1->GetBlender()->GetAnimationSet()->GetAnimation("Jump"), false, 2.0f, "Jump");
		mageAnim1->AddState(mageJump);
		//State* mageLand = new State();
		//mageLand->Init(mageAnim1, mageAnim1->GetBlender()->GetAnimationSet()->GetAnimation("Land"), false, 2.0f, "Land");
		//mageAnim1->AddState(mageLand);
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
		Param::Trigger* jumpTrigger = new Param::Trigger();
		jumpTrigger->Init("Jump", false);
		mageAnim1->AddParameter(jumpTrigger);
		Transition* idleToRun = new Transition();
		mageIdle->AddTransition(idleToRun);
		idleToRun->Init(mageIdle, mageRun, -1, 0.5f);
		idleToRun->AddCondition(runTrigger);
		Transition* runToIdle = new Transition();
		mageRun->AddTransition(runToIdle);
		runToIdle->Init(mageRun, mageIdle, -1, 0.5f);
		runToIdle->AddCondition(idleTrigger);
		Transition* idleToStumble = new Transition();
		mageIdle->AddTransition(idleToStumble);
		idleToStumble->Init(mageIdle, mageStumble, -1, 0.01f);
		idleToStumble->AddCondition(stumbleTrigger);
		Transition* stumbleToIdle = new Transition();
		mageStumble->AddTransition(stumbleToIdle);
		stumbleToIdle->Init(mageStumble, mageIdle, 0, 0.01f); //exit time of 0 will make transition happen right when cur animation is done
		Transition* idleToJump = new Transition();
		mageIdle->AddTransition(idleToJump);
		idleToJump->Init(mageIdle, mageJump, -1, 0.1f);
		idleToJump->AddCondition(jumpTrigger);
		Transition* jumpToRun = new Transition();
		mageJump->AddTransition(jumpToRun);
		jumpToRun->Init(mageJump, mageRun, -1, 0.1f);
		jumpToRun->AddCondition(runTrigger);
		Transition* jumpToIdle = new Transition();
		mageJump->AddTransition(jumpToIdle);
		jumpToIdle->Init(mageJump, mageIdle, 0, 0.1f);
		//Transition* jumpToLand = new Transition();
		//mageJump->AddTransition(jumpToLand);
		//jumpToLand->Init(mageJump, mageLand, 0, 0.0f);
		//Transition* landToIdle = new Transition();
		//mageLand->AddTransition(landToIdle);
		//landToIdle->Init(mageLand, mageIdle, 0, 0.001f);
		Transition* runToJump = new Transition();
		mageRun->AddTransition(runToJump);
		runToJump->Init(mageRun, mageJump, -1, 0.001f);
		runToJump->AddCondition(jumpTrigger);

		string cameraName = "Camera";
		cameraName += to_string(i);

		//Transform* mageHead = new Transform();
		//mageRenderer1->GetBlender()->GetAnimationSet()->GetSkeleton()->GetBone("Head")->

		GameObject* camera1 = new GameObject();
		basic->AddGameObject(camera1);
		camera1->Init(cameraName);
		camera1->InitTransform(identity, { 0, 5.0f, 0.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage1->GetTransform(), nullptr, nullptr);
		//camera1->InitTransform(identity, { 0, 1.6f, 0.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage1->GetTransform(), nullptr, nullptr);
		//camera1->InitTransform(identity, { 0, 0, -1.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage1->GetTransform(), nullptr, nullptr);
		//camera1->InitTransform(identity, { 0, 0, -15.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage1->GetTransform(), nullptr, nullptr);
		Camera* cameraController1 = new Camera();
		camera1->AddComponent(cameraController1);
		cameraController1->Init({ 0.0f, 0.7f, 1.5f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

		string crosseName = "Crosse";
		crosseName += to_string(i);

		crosse->Init(crosseName);
		crosse->InitTransform(identity, { 0, 0.25f, 1.2f }, { 0, 0, 0 }, { 1, 1, 1 }, camera1->GetTransform(), nullptr, nullptr);
		//crosse->InitTransform(identity, { 0, 5.4f, -1.7f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage1->GetTransform(), nullptr, nullptr);
		SphereCollider* crosseNetCollider = new SphereCollider(0.75f, crosse, true);
		crosse->AddSphereCollider(crosseNetCollider);
		Renderer* crosseRenderer = new Renderer();
		crosse->AddComponent(crosseRenderer);
		crosseRenderer->Init("Crosse", "Crosse", "Static", "", "", projection, devResources);
		if (i <= 4)
			crosseRenderer->SetTeamColor({ 1,0,0,0 });
		else
			crosseRenderer->SetTeamColor({ 0,0,1,0 });
		Crosse* crosseController = new Crosse();
		crosse->AddComponent(crosseController);
		crosseController->Init();
	}

	ballController->LateInit();

	//for max poly count
	//for (int i = 0; i < 15; ++i)
	//{
	//	for (int j = 0; j < 15; ++j)
	//	{
	//		GameObject* mage1 = new GameObject();
	//		basic->AddGameObject(mage1);
	//		mage1->Init("MageCrap");

	//		mage1->InitTransform(identity, { (float)0 + i * 2.0f, 10, -40.0f + j * 2.0f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);

	//		Renderer* mageRenderer1 = new Renderer();
	//		mage1->AddComponent(mageRenderer1);
	//		mageRenderer1->Init("Mage", "NormalMapped", "Bind", "", "Idle", projection, devResources);
	//		if (i <= 4)
	//			mageRenderer1->SetTeamColor({ 1,0,0,0 });
	//		else
	//			mageRenderer1->SetTeamColor({ 0,0,1,0 });
	//	}
	//}
	//for (int i = 0; i < ai.size(); ++i)
	//{
	//	ai[i]->Init(goal, goal2);
	//}

	objIDs[8] = (UINT8)basic->GetNumObjects();
	basic->AddGameObject(goal);
	goal->Init("Goal");
	goal->InitTransform(identity, { -7, 15, (float)-row + 1.5f }, { 0,0,0 }, { 1,1,1 }, nullptr, nullptr, nullptr);
	Renderer* GoalRenderer = new Renderer();
	goal->AddComponent(GoalRenderer);
	GoalRenderer->Init("WallGoal", "Static", "Static", "", "", projection, devResources);
	BoxCollider* Goal1col = new BoxCollider(goal, true, { 7,4,2 }, { -7,-4,-3 });
	goal->AddBoxCollider(Goal1col);
	Goal* g1 = new Goal(goal);
	goal->AddComponent(g1);

	objIDs[9] = (UINT8)basic->GetNumObjects();
	basic->AddGameObject(goal2);
	goal2->Init("Goal2");
	goal2->InitTransform(identity, { -7, 15, (float)row - 43 }, { 0, 3.14159f, 0 }, { 1,1,1 }, nullptr, nullptr, nullptr);
	Renderer* GoalRenderer2 = new Renderer();
	goal2->AddComponent(GoalRenderer2);
	GoalRenderer2->Init("WallGoal", "Static", "Static", "", "", projection, devResources);
	BoxCollider* Goal2col = new BoxCollider(goal2, true, { 7,4,2 }, { -7,-4,-3 });
	goal2->AddBoxCollider(Goal2col);
	Goal* g2 = new Goal(goal2);
	goal2->AddComponent(g2);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GameObject* testPlayer = new GameObject();
	basic->AddGameObject(testPlayer);
	testPlayer->Init("TestPlayer");
	testPlayer->InitTransform(identity, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* testPlayerRenderer = new Renderer();
	testPlayer->AddComponent(testPlayerRenderer);
	testPlayerRenderer->Init("TestPlayer", "Static", "Static", "", "", projection, devResources);

	//for (int j = 0; j < 11; ++j)
	//{
	//	for (int i = 0; i < 11; ++i)
	//	{
	//		GameObject* testball = new GameObject();
	//		basic->AddGameObject(testball);
	//		testball->Init("testball");
	//		testball->InitTransform(identity, { 1.5f * -j + 0, 2, 1.5f * -i + 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	//		Renderer* testballRenderer = new Renderer();
	//		testball->AddComponent(testballRenderer);
	//		testballRenderer->Init("HighDetalBall", "Static", "Static", "", "", projection, devResources);
	//	}
	//}

	GameObject* Wall = new GameObject();
	basic->AddGameObject(Wall);
	Wall->Init("Wall");
	Wall->InitTransform(identity, { 57, 0, 0 }, { 0, 0, 0 }, { 1, 100, 600 }, nullptr, nullptr, nullptr);
	Renderer* WallRenderer = new Renderer();
	Wall->AddComponent(WallRenderer);
	WallRenderer->Init("MeterBox", "Static", "Static", "", "", projection, devResources);
	BoxCollider* Wallboxcol = new BoxCollider(Wall, false, { 10.5f,300,300 }, { -0.5f,-300,-300 });
	Wall->AddBoxCollider(Wallboxcol);

	GameObject* Wall2 = new GameObject();
	basic->AddGameObject(Wall2);
	Wall2->Init("Wall2");
	Wall2->InitTransform(identity, { -70, 0, 0 }, { 0, 0, 0 }, { 1, 100, 600 }, nullptr, nullptr, nullptr);
	Renderer* WallRenderer2 = new Renderer();
	Wall2->AddComponent(WallRenderer2);
	WallRenderer2->Init("MeterBox", "Static", "Static", "", "", projection, devResources);
	BoxCollider* Wallboxcol2 = new BoxCollider(Wall2, false, { 0.5f,300,300 }, { -10.5f,-300,-300 });
	Wall2->AddBoxCollider(Wallboxcol2);

	GameObject* Wall3 = new GameObject();
	basic->AddGameObject(Wall3);
	Wall3->Init("Wall3");
	Wall3->InitTransform(identity, { 0, 0, 38.5f }, { 0, 0, 0 }, { 600, 100, 1 }, nullptr, nullptr, nullptr);
	Renderer* WallRenderer3 = new Renderer();
	Wall3->AddComponent(WallRenderer3);
	WallRenderer3->Init("MeterBox", "Static", "Static", "", "", projection, devResources);
	BoxCollider* Wallboxcol3 = new BoxCollider(Wall3, false, { 300,300, 0.5f }, { -300,-300,-0.5f });
	Wall3->AddBoxCollider(Wallboxcol3);

	GameObject* Wall4 = new GameObject();
	basic->AddGameObject(Wall4);
	Wall4->Init("Wall4");
	Wall4->InitTransform(identity, { 0, 0, -80.5f }, { 0, 0, 0 }, { 600, 100, 1 }, nullptr, nullptr, nullptr);
	Renderer* WallRenderer4 = new Renderer();
	Wall4->AddComponent(WallRenderer4);
	WallRenderer4->Init("MeterBox", "Static", "Static", "", "", projection, devResources);
	BoxCollider* Wallboxcol4 = new BoxCollider(Wall4, false, { 300,300, 0.5f }, { -300,-300,-0.5f });
	Wall4->AddBoxCollider(Wallboxcol4);

	GameObject* meterbox6 = new GameObject();
	basic->AddGameObject(meterbox6);
	meterbox6->Init("MeterBox6");
	meterbox6->InitTransform(identity, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* meterboxRenderer6 = new Renderer();
	meterbox6->AddComponent(meterboxRenderer6);
	meterboxRenderer6->Init("MeterBox", "Static", "Static", "", "", projection, devResources);
	BoxCollider* meterboxcol6 = new BoxCollider(meterbox6, false, { 300,0.2f,300 }, { -300,-30,-300 });
	meterbox6->AddBoxCollider(meterboxcol6);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float3* floor = CreateFloor(2.0f, row, col, float3((float)-row, -10, (float)-col));

	GameObject* HexFloor = new GameObject();
	basic->AddGameObject(HexFloor);
	HexFloor->Init("HexFloor");
	HexFloor->InitTransform(identity, { 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
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
	if (!DEBUG_GRAPHICS) {
		CreateUI(basic);
	}
}

void Game::CreateUI(Scene * basic)
{
	// trapezoid backing
	GameObject * scoreA = new GameObject();
	basic->AddUIObject(scoreA);
	scoreA->Init("gameScoreBase");
	Button * theSButton = new Button(true, true, L"05:00", (unsigned int)strlen("05:00"), 500.0f, 100.0f, devResources, 0);
	theSButton->SetGameObject(scoreA);
	theSButton->setTimer(true);
	theSButton->showFPS(false);
	theSButton->setPositionMultipliers(0.5f, 0.0f);
	scoreA->AddComponent(theSButton);
	UIRenderer * scoreRender = new UIRenderer();
	scoreRender->Init(true, 35.0f, devResources, theSButton, L"Consolas", D2D1::ColorF(0.8f, 0.8f, 0.8f, 1.0f));
	scoreRender->DecodeBitmap(L"../Assets/UI/trapezoid.png");
	scoreA->AddComponent(scoreRender);
	scoreRender->MakeRTSize();
	theSButton->MakeRect();
	scoreRender->InitMetrics();
	theSButton->setOrigin();


	GameObject * scoreB = new GameObject();
	basic->AddUIObject(scoreB);
	scoreB->Init("gameScoreA");
	Button * theSButtonB = new Button(true, true, L"0", (unsigned int)strlen("0"), 60.0f, 60.0f, devResources, 0);
	theSButtonB->SetGameObject(scoreB);
	theSButtonB->showFPS(false);
	theSButtonB->setPositionMultipliers(0.40f, 0.11f);
	scoreB->AddComponent(theSButtonB);
	UIRenderer * scoreBRender = new UIRenderer();
	scoreBRender->Init(true, 30.0f, devResources, theSButtonB, L"Consolas", D2D1::ColorF::Black);
	scoreBRender->DecodeBitmap(L"../Assets/UI/smallHexR.png");
	scoreB->AddComponent(scoreBRender);
	scoreBRender->MakeRTSize();
	theSButtonB->MakeRect();
	scoreBRender->InitMetrics();
	theSButtonB->setOrigin();


	GameObject * scoreC = new GameObject();
	basic->AddUIObject(scoreC);
	scoreC->Init("gameScoreB");
	Button * theSButtonC = new Button(true, true, L"0", (unsigned int)strlen("0"), 60.0f, 60.0f, devResources, 0);
	theSButtonC->SetGameObject(scoreC);
	theSButtonC->showFPS(false);
	theSButtonC->setPositionMultipliers(0.6f, 0.11f);
	scoreC->AddComponent(theSButtonC);
	UIRenderer * scoreCRender = new UIRenderer();
	scoreCRender->Init(true, 30.0f, devResources, theSButtonC, L"Consolas", D2D1::ColorF::Black);
	scoreCRender->DecodeBitmap(L"../Assets/UI/smallHexB.png");
	scoreC->AddComponent(scoreCRender);
	scoreCRender->MakeRTSize();
	theSButtonC->MakeRect();
	theSButtonC->setOrigin();
	scoreCRender->InitMetrics();

	GameObject * sprintBar = new GameObject();
	basic->AddUIObject(sprintBar);
	sprintBar->Init("sprintBar");
	MeterBar * sprintMeter = new MeterBar(false, 200.0f, 20.0f, 0.2f, 0.95f);
	sprintBar->AddComponent(sprintMeter);
	sprintMeter->MakeHandler();
	UIRenderer * sprintRender = new UIRenderer();
	sprintRender->Init(false, devResources, sprintMeter);
	sprintRender->DecodeBitmap(L"../Assets/UI/meterBar.png");
	sprintRender->DecodeBitmap(L"../Assets/UI/meterBorder.png");
	sprintBar->AddComponent(sprintRender);
	sprintRender->MakeRTSize();
	sprintMeter->MakeRects();
	sprintMeter->setDrainTime(5.0f);
	sprintMeter->setRechargeTime(10.0f);
	sprintMeter->setCanRecharge(true);

	CreatePauseMenu(basic);

	#ifdef DEBUG
		GameObject * debugUI = new GameObject();
		basic->AddUIObject(debugUI);
		debugUI->Init("debugUI");
		Button * theButton = new Button(true, true, L"Titans with Sticks", (unsigned int)strlen("Titans with Sticks"), 350.0f, 100.0f, devResources, 0);
		theButton->SetGameObject(debugUI);
		theButton->showFPS(true);
		theButton->setPositionMultipliers(0.0f, 0.2f);
		debugUI->AddComponent(theButton);
		UIRenderer * buttonRender = new UIRenderer();
		buttonRender->Init(true, 30.0f, devResources, theButton, L"Consolas", D2D1::ColorF::Black);
		debugUI->AddComponent(buttonRender);
		theButton->MakeRect();
		theButton->setOrigin();
	#endif
}

void Game::CreateMenu(Scene * scene)
{
	// background
	GameObject * bg = new GameObject();
	scene->AddUIObject(bg);
	bg->Init("background");
	Button * bgButton = new Button(true, false, L"", 0, 1000.0f, 750.0f, devResources, 0);
	bgButton->SetGameObject(bg);
	bgButton->showFPS(false);
	bgButton->setPositionMultipliers(0.5f, 0.5f);
	bg->AddComponent(bgButton);
	UIRenderer * bgRender = new UIRenderer();
	bgRender->Init(true, 35.0f, devResources, bgButton, L"", D2D1::ColorF::Black);
	bgRender->DecodeBitmap(L"../Assets/UI/main_menu.png");
	bg->AddComponent(bgRender);
	bgRender->MakeRTSize();
	bgButton->MakeRect();

	// title
	GameObject * title = new GameObject();
	scene->AddUIObject(title);
	title->Init("title");
	Button * tButton = new Button(true, false, L"", 0, 450.0f, 236.0f, devResources, 0);
	tButton->SetGameObject(title);
	tButton->showFPS(false);
	tButton->setPositionMultipliers(0.65f, 0.25f);
	title->AddComponent(tButton);
	UIRenderer * tRender = new UIRenderer();
	tRender->Init(true, 35.0f, devResources, tButton, L"", D2D1::ColorF::Black);
	tRender->DecodeBitmap(L"../Assets/UI/newTitle.png");
	title->AddComponent(tRender);
	tRender->MakeRTSize();
	tButton->MakeRect();

	// solo button
	GameObject * soloPlayer = new GameObject();
	scene->AddUIObject(soloPlayer);
	soloPlayer->Init("soloPlayer");
	Button * sButton = new Button(true, true, L"", (unsigned int)strlen(""), 300.0f, 60.0f, devResources, 3);
	sButton->SetGameObject(soloPlayer);
	sButton->showFPS(false);
	sButton->setPositionMultipliers(0.65f, 0.45f);
	soloPlayer->AddComponent(sButton);
	UIRenderer * sRender = new UIRenderer();
	sRender->Init(true, 25.0f, devResources, sButton, L"Brush Script MT", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
	sRender->DecodeBitmap(L"../Assets/UI/playButton.png");
	sRender->DecodeBitmap(L"../Assets/UI/playButton2.png");
	soloPlayer->AddComponent(sRender);
	sRender->MakeRTSize();
	sButton->MakeRect();
	sButton->MakeHandler();
	sRender->InitMetrics();

	// host button
	GameObject * multiPlayer = new GameObject();
	scene->AddUIObject(multiPlayer);
	multiPlayer->Init("multiHost");
	Button * mButton = new Button(true, true, L"", (unsigned int)strlen(""), 150.0f, 60.0f, devResources, 1);
	mButton->SetGameObject(multiPlayer);
	mButton->showFPS(false);
	mButton->setPositionMultipliers(0.56f, 0.53f);
	multiPlayer->AddComponent(mButton);
	UIRenderer * mRender = new UIRenderer();
	mRender->Init(true, 25.0f, devResources, mButton, L"Bradley Hand ITC", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
	mRender->DecodeBitmap(L"../Assets/UI/hostButton.png");
	mRender->DecodeBitmap(L"../Assets/UI/hostButton2.png");
	multiPlayer->AddComponent(mRender);
	mRender->MakeRTSize();
	mButton->MakeRect();
	mButton->MakeHandler();
	mRender->InitMetrics();

	// join button
	GameObject * multiPlayer2 = new GameObject();
	scene->AddUIObject(multiPlayer2);
	multiPlayer->Init("multiJoin");
	Button * mButton2 = new Button(true, true, L"", (unsigned int)strlen(""), 150.0f, 60.0f, devResources, 2);
	mButton2->SetGameObject(multiPlayer2);
	mButton2->showFPS(false);
	mButton2->setPositionMultipliers(0.725f, 0.53f);
	multiPlayer2->AddComponent(mButton2);
	UIRenderer * mRender2 = new UIRenderer();
	mRender2->Init(true, 25.0f, devResources, mButton2, L"Bradley Hand ITC", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
	mRender2->DecodeBitmap(L"../Assets/UI/joinButton.png");
	mRender2->DecodeBitmap(L"../Assets/UI/joinButton2.png");
	multiPlayer2->AddComponent(mRender2);
	mRender2->MakeRTSize();
	mButton2->MakeRect();
	mButton2->MakeHandler();
	mRender2->InitMetrics();

	// credits
	GameObject * credits = new GameObject();
	scene->AddUIObject(credits);
	credits->Init("credits");
	Button * cButton = new Button(true, true, L"", (unsigned int)strlen(""), 150.0f, 60.0f, devResources, 4);
	cButton->SetGameObject(credits);
	cButton->showFPS(false);
	cButton->setPositionMultipliers(0.56f, 0.62f);
	credits->AddComponent(cButton);
	UIRenderer * cRender = new UIRenderer();
	cRender->Init(true, 25.0f, devResources, cButton, L"Freestyle Script", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
	cRender->DecodeBitmap(L"../Assets/UI/creditsButton.png");
	cRender->DecodeBitmap(L"../Assets/UI/creditsButton2.png");
	credits->AddComponent(cRender);
	cRender->MakeRTSize();
	cButton->MakeRect();
	cButton->MakeHandler();
	cRender->InitMetrics();

	// exit
	GameObject * exit = new GameObject();
	scene->AddUIObject(exit);
	exit->Init("exit");
	Button * eButton = new Button(true, true, L"", (unsigned int)strlen(""), 150.0f, 60.0f, devResources, 5);
	eButton->SetGameObject(exit);
	eButton->showFPS(false);
	eButton->setPositionMultipliers(0.725f, 0.62f);
	exit->AddComponent(eButton);
	UIRenderer * eRender = new UIRenderer();
	eRender->Init(true, 25.0f, devResources, eButton, L"Freestyle Script", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
	eRender->DecodeBitmap(L"../Assets/UI/exitButton.png");
	eRender->DecodeBitmap(L"../Assets/UI/exitButton2.png");
	exit->AddComponent(eRender);
	eRender->MakeRTSize();
	eButton->MakeRect();
	eButton->MakeHandler();
	eRender->InitMetrics();


	// background 2.0
	GameObject * bg2 = new GameObject();
	scene->AddUIObject(bg2);
	bg2->Init("background2");
	Button * bg2Button = new Button(true, false, L"", 0, 1000.0f, 750.0f, devResources, 0);
	bg2Button->SetGameObject(bg2);
	bg2Button->showFPS(false);
	bg2Button->setPositionMultipliers(0.5f, 0.5f);
	bg2->AddComponent(bg2Button);
	UIRenderer * bg2Render = new UIRenderer();
	bg2Render->Init(true, 35.0f, devResources, bgButton, L"", D2D1::ColorF::Black);
	bg2Render->DecodeBitmap(L"../Assets/UI/main_menu2.png");
	bg2->AddComponent(bg2Render);
	bg2Render->MakeRTSize();
	bg2Button->MakeRect();


}

void Game::CreateLobby(Scene * scene)
{
	// background
	GameObject * bg = new GameObject();
	scene->AddUIObject(bg);
	bg->Init("background");
	Button * bgButton = new Button(true, false, L"", 0, 1000.0f, 750.0f, devResources, 0);
	bgButton->setSceneIndex((unsigned int)scenes.size());
	bgButton->SetGameObject(bg);
	bgButton->showFPS(false);
	bgButton->setPositionMultipliers(0.5f, 0.5f);
	bg->AddComponent(bgButton);
	UIRenderer * bgRender = new UIRenderer();
	bgRender->Init(true, 35.0f, devResources, bgButton, L"", D2D1::ColorF::Black);
	bgRender->DecodeBitmap(L"../Assets/UI/main_menu.png");
	bg->AddComponent(bgRender);
	bgRender->MakeRTSize();
	bgButton->MakeRect();

	// start game, will only show if isServer
	GameObject * startGame = new GameObject();
	scene->AddUIObject(startGame);
	startGame->Init("Start");
	Button * sButton = new Button(true, true, L"", (unsigned int)strlen(""), 300.0f, 60.0f, devResources, 3);
	sButton->setSceneIndex((unsigned int)scenes.size());
	sButton->SetGameObject(startGame);
	sButton->showFPS(false);
	sButton->setPositionMultipliers(0.55f, 0.55f);
	startGame->AddComponent(sButton);
	UIRenderer * sRender = new UIRenderer();
	sRender->Init(true, 25.0f, devResources, sButton, L"Brush Script MT", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
	sRender->DecodeBitmap(L"../Assets/UI/playButton.png");
	sRender->DecodeBitmap(L"../Assets/UI/playButton2.png");
	startGame->AddComponent(sRender);
	sRender->MakeRTSize();
	sButton->MakeRect();
	sButton->MakeHandler();
	sRender->InitMetrics();

	// start game, will only show if isServer
	GameObject * exitGame = new GameObject();
	scene->AddUIObject(exitGame);
	exitGame->Init("Exit");
	Button * eButton = new Button(true, true, L"", (unsigned int)strlen(""), 300.0f, 60.0f, devResources, 6);
	eButton->setSceneIndex((unsigned int)scenes.size());
	eButton->SetGameObject(exitGame);
	eButton->showFPS(false);
	eButton->setPositionMultipliers(0.55f, 0.65f);
	exitGame->AddComponent(eButton);
	UIRenderer * eRender = new UIRenderer();
	eRender->Init(true, 25.0f, devResources, eButton, L"Brush Script MT", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
	eRender->DecodeBitmap(L"../Assets/UI/returnButton.png");
	eRender->DecodeBitmap(L"../Assets/UI/returnButton2.png");
	exitGame->AddComponent(eRender);
	eRender->MakeRTSize();
	eButton->MakeRect();
	eButton->MakeHandler();
	eRender->InitMetrics();

	// number of players
	GameObject * numPlayers = new GameObject();
	scene->AddUIObject(numPlayers);
	numPlayers->Init("Players");
	Button * nButton = new Button(true, false, L"1/4", (unsigned int)strlen("1/4"), 150.0f, 150.0f, devResources, 0);
	nButton->setSceneIndex((unsigned int)scenes.size());
	nButton->SetGameObject(numPlayers);
	nButton->showFPS(false);
	nButton->setPositionMultipliers(0.5f, 0.3f);
	numPlayers->AddComponent(nButton);
	UIRenderer * nRender = new UIRenderer();
	nRender->Init(false, 80.0f, devResources, nButton, L"Consolas", D2D1::ColorF(1.0f, 0.412f, 0.706f, 1.0f));
	numPlayers->AddComponent(nRender);
	nRender->MakeRTSize();
	nButton->MakeRect();
	nButton->setOrigin();
	nButton->MakeHandler();
	nRender->InitMetrics();

	// change team to A
	GameObject * changeTeamA = new GameObject();
	scene->AddUIObject(changeTeamA);
	changeTeamA->Init("ChangeA");
	Button * caButton = new Button(true, true, L"", (unsigned int)strlen(""), 90.0f, 90.0f, devResources, 8);
	caButton->setSceneIndex((unsigned int)scenes.size());
	caButton->SetGameObject(changeTeamA);
	caButton->showFPS(false);
	caButton->setPositionMultipliers(0.8f, 0.45f);
	changeTeamA->AddComponent(caButton);
	UIRenderer * eaRender = new UIRenderer();
	eaRender->Init(true, 25.0f, devResources, caButton, L"Brush Script MT", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
	eaRender->DecodeBitmap(L"../Assets/UI/smallHexR.png");
	eaRender->DecodeBitmap(L"../Assets/UI/smallHexR2.png");
	changeTeamA->AddComponent(eaRender);
	eaRender->MakeRTSize();
	caButton->MakeRect();
	caButton->MakeHandler();
	eaRender->InitMetrics();
	caButton->setHelper(scene->GetNumUIObjects());

	// change team to B
	GameObject * changeTeamB = new GameObject();
	scene->AddUIObject(changeTeamB);
	changeTeamB->Init("ChangeB");
	Button * cbButton = new Button(true, true, L"", (unsigned int)strlen(""), 90.0f, 90.0f, devResources, 9);
	cbButton->setSceneIndex((unsigned int)scenes.size());
	cbButton->SetGameObject(changeTeamB);
	cbButton->showFPS(false);
	cbButton->setPositionMultipliers(0.7f, 0.4f);
	changeTeamB->AddComponent(cbButton);
	UIRenderer * ebRender = new UIRenderer();
	ebRender->Init(true, 25.0f, devResources, cbButton, L"Brush Script MT", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
	ebRender->DecodeBitmap(L"../Assets/UI/smallHexB.png");
	ebRender->DecodeBitmap(L"../Assets/UI/smallHexB2.png");
	changeTeamB->AddComponent(ebRender);
	ebRender->MakeRTSize();
	cbButton->MakeRect();
	cbButton->MakeHandler();
	ebRender->InitMetrics();
	cbButton->setHelper(scene->GetNumUIObjects() - 2);

	// change team logo
	GameObject * changeTeam = new GameObject();
	scene->AddUIObject(changeTeam);
	changeTeam->Init("changeLogo");
	Button * ctButton = new Button(true, true, L"", (unsigned int)strlen(""), 246.0f, 200.0f, devResources, 0);
	ctButton->setSceneIndex((unsigned int)scenes.size());
	ctButton->SetGameObject(changeTeam);
	ctButton->showFPS(false);
	ctButton->setPositionMultipliers(0.75f, 0.25f);
	changeTeam->AddComponent(ctButton);
	UIRenderer * ctRender = new UIRenderer();
	ctRender->Init(true, 25.0f, devResources, ctButton, L"Brush Script MT", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
	ctRender->DecodeBitmap(L"../Assets/UI/changeTeam.png");
	changeTeam->AddComponent(ctRender);
	ctRender->MakeRTSize();
	ctButton->MakeRect();
	ctButton->MakeHandler();
	ctRender->InitMetrics();

	// background 2.0
	GameObject * bg2 = new GameObject();
	scene->AddUIObject(bg2);
	bg2->Init("background2");
	Button * bg2Button = new Button(true, false, L"", 0, 1000.0f, 750.0f, devResources, 0);
	bg2Button->setSceneIndex((unsigned int)scenes.size());
	bg2Button->SetGameObject(bg2);
	bg2Button->showFPS(false);
	bg2Button->setPositionMultipliers(0.5f, 0.5f);
	bg2->AddComponent(bg2Button);
	UIRenderer * bg2Render = new UIRenderer();
	bg2Render->Init(true, 35.0f, devResources, bgButton, L"", D2D1::ColorF::Black);
	bg2Render->DecodeBitmap(L"../Assets/UI/main_menu2.png");
	bg2->AddComponent(bg2Render);
	bg2Render->MakeRTSize();
	bg2Button->MakeRect();
}

void Game::CreatePauseMenu(Scene * scene)
{
	// server only?
	GameObject * resumeGame = new GameObject();
	scene->AddUIObject(resumeGame);
	resumeGame->Init("pauseResume");
	Button * rButton = new Button(true, true, L"", (unsigned int)strlen(""), 175.0f, 70.0f, devResources, 7);
	rButton->setSceneIndex((unsigned int)scenes.size());
	rButton->SetGameObject(resumeGame);
	rButton->showFPS(false);
	rButton->setPositionMultipliers(0.80f, 0.30f);
	resumeGame->AddComponent(rButton);
	UIRenderer * rRender = new UIRenderer();
	rRender->Init(true, 25.0f, devResources, rButton, L"Brush Script MT", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
	rRender->DecodeBitmap(L"../Assets/UI/resumeButton.png");
	rRender->DecodeBitmap(L"../Assets/UI/resumeButton2.png");
	resumeGame->AddComponent(rRender);
	rRender->MakeRTSize();
	rButton->MakeRect();
	rButton->MakeHandler();
	rRender->InitMetrics();
	rButton->SetActive(false);
	rButton->setHelper(scene->GetNumUIObjects());

	// exit, closes application
	GameObject * exitGame = new GameObject();
	scene->AddUIObject(exitGame);
	exitGame->Init("pauseExit");
	Button * eButton = new Button(true, true, L"", (unsigned int)strlen(""), 175.0f, 70.0f, devResources, 5);
	eButton->setSceneIndex((unsigned int)scenes.size());
	eButton->SetGameObject(exitGame);
	eButton->showFPS(false);
	eButton->setPositionMultipliers(0.80f, 0.40f);
	exitGame->AddComponent(eButton);
	UIRenderer * eRender = new UIRenderer();
	eRender->Init(true, 25.0f, devResources, eButton, L"Brush Script MT", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
	eRender->DecodeBitmap(L"../Assets/UI/quitButton.png");
	eRender->DecodeBitmap(L"../Assets/UI/quitButton2.png");
	exitGame->AddComponent(eRender);
	eRender->MakeRTSize();
	eButton->MakeRect();
	eButton->MakeHandler();
	eRender->InitMetrics();
	eButton->SetActive(false);

	// exit, return to menu
	rButton->setHelper(scene->GetNumUIObjects());
	GameObject * exitMenu = new GameObject();
	scene->AddUIObject(exitMenu);
	exitMenu->Init("pauseMenu");
	Button * mButton = new Button(true, true, L"", (unsigned int)strlen(""), 175.0f, 70.0f, devResources, 6);
	mButton->setSceneIndex((unsigned int)scenes.size());
	mButton->SetGameObject(exitMenu);
	mButton->showFPS(false);
	mButton->setPositionMultipliers(0.80f, 0.50f);
	exitMenu->AddComponent(mButton);
	UIRenderer * mRender = new UIRenderer();
	mRender->Init(true, 25.0f, devResources, mButton, L"Brush Script MT", D2D1::ColorF(0.196f, 0.804f, 0.196f, 1.0f));
	mRender->DecodeBitmap(L"../Assets/UI/menuButton.png");
	mRender->DecodeBitmap(L"../Assets/UI/menuButton2.png");
	exitMenu->AddComponent(mRender);
	mRender->MakeRTSize();
	mButton->MakeRect();
	mButton->MakeHandler();
	mRender->InitMetrics();
	mButton->SetActive(false);
}

void Game::AssignPlayers()
{
	vector<AI*> ai;
	if (ResourceManager::GetSingleton()->IsMultiplayer())
	{
		if (ResourceManager::GetSingleton()->IsServer())
		{
			// get teams of all players
			// all others become ai

			for (unsigned int i = 0; i < 8; ++i) 
			{
				if (!server.isPlayer(i)) 
				{
					GameObject * mage1 = scenes[2]->GetGameObjects(objIDs[i]);
					AI *mageAI = new AI(mage1);
					mage1->AddComponent(mageAI);
					ai.push_back(mageAI);
				}
			}
			GameObject * goal = scenes[2]->GetGameObjects(objIDs[8]);
			GameObject * goal2 = scenes[2]->GetGameObjects(objIDs[9]);

			for (int i = 0; i < ai.size(); ++i)
			{
				ai[i]->Init(goal, goal2);
			}
		}
	}
	else
	{
		// find team player selected
		for (unsigned int i = 0; i < 8; ++i) 
		{
			// set mage 1 to player if red team
			if (team == TEAM_A && i != 0)
			{
				GameObject * mage1 = scenes[2]->GetGameObjects(objIDs[i]);
				AI *mageAI = new AI(mage1);
				mage1->AddComponent(mageAI);
				ai.push_back(mageAI);
			}
			// set mage 5 to player if blue team
			else if (team == TEAM_B && i!= 4)
			{
				GameObject * mage1 = scenes[2]->GetGameObjects(objIDs[i]);
				AI *mageAI = new AI(mage1);
				mage1->AddComponent(mageAI);
				ai.push_back(mageAI);
			}
		}
		GameObject * goal = scenes[2]->GetGameObjects(objIDs[8]);
		GameObject * goal2 = scenes[2]->GetGameObjects(objIDs[9]);

		for (int i = 0; i < ai.size(); ++i)
		{
			ai[i]->Init(goal, goal2);
		}
	}

}

void Game::EnableButton(std::string name, bool toggle)
{
	GameObject * obj = scenes[currentScene]->GetUIByName(name);
	Button * button = obj->GetComponent<Button>();
	button->SetEnabled(toggle);
	button->SetActive(toggle);
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

		//parent info
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

		//animation info
		int animIndex = -1;
		int transitionIndex = -1;
		AnimatorController* animator = gameObject->GetComponent<AnimatorController>();

		if (animator)
		{
			animIndex = animator->GetNextStateIndex();

			if (animIndex >= 0)
			{
				transitionIndex = animator->GetState(animIndex)->GetTransitionIndex();

				if (animIndex == 2)
				{
					int breakpoint = 69;
					breakpoint += 420;
				}
			}
			//animIndex = animator->GetCurrentStateIndex();
		}

		state->animationIndex = animIndex;
		state->transitionIndex = transitionIndex;

		//sound info is handled by event
	}

	server.SetGameStates(gameStates);

	//reset sound info after server sets game states
	for (int i = 0; i < gameStates.size(); ++i)
	{
		gameStates[i]->soundID = -1;
		gameStates[i]->hasSound = false;
	}
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

				gameObject->GetTransform()->SetPosition({ position.x, position.y, position.z });
				gameObject->GetTransform()->SetRotation({ rotation.x, rotation.y, rotation.z });

				INT8 parentIndex = client.GetParentIndex(i);
				if (parentIndex >= 0)
				{
					gameObject->GetTransform()->SetParent((*gameObjects)[parentIndex]->GetTransform());
				}

				INT8 animIndex = client.GetAnimationIndex(i);
				INT8 transitionIndex = client.GetTransitionIndex(i);

				if (animIndex >= 0)
				{
					AnimatorController* animator = gameObject->GetComponent<AnimatorController>();

					if (animator)
					{
						if (animator->GetNextStateIndex() != animIndex) //only transition if it's not already transitioning
						{
							//cout << to_string(animIndex) << endl;

							if (animIndex == 2)
							{
								//cout << "Client received stumble index" << endl;
							}

							animator->TransitionTo(animIndex, transitionIndex);
						}
					}
				}

				bool hasSound = client.HasSound(i);

				if (hasSound)
				{
					INT32 soundID = client.GetSoundID(i);
					soundEngine->PostEvent(soundID, i);

					gameStates[i]->soundID = -1;
					gameStates[i]->hasSound = false;

					//cout << "play sound" << endl;
				}
				Crosse* crosse = gameObject->GetComponent<Crosse>();
				if(crosse)
					crosse->SetColor(client.hasBall(i));
			}
		}
	}
}

void Game::UpdateScoreUI()
{
	GameObject * scoreA = scenes[currentScene]->GetUIByName("gameScoreA");
	Button * buttonA = scoreA->GetComponent<Button>();
	buttonA->setText(to_wstring(Team1Score));

	GameObject * scoreB = scenes[currentScene]->GetUIByName("gameScoreB");
	Button * buttonB = scoreB->GetComponent<Button>();
	buttonB->setText(to_wstring(Team2Score));
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

	if (currentScene == 1)
	{
		if (ResourceManager::GetSingleton()->IsServer())
			EnableButton("Start", true);
		else
			EnableButton("Start", false);
	}
	else if (currentScene == 2)
	{
		AssignPlayers();
	}

	//resize gamestates
	/*for (unsigned int i = 0; i < gameStates.size(); ++i)
	{
		delete gameStates[i];
	}

	gameStates.clear();*/
	gameStates.resize(scenes[currentScene]->GetNumObjects());
	for (int i = 0; i < gameStates.size(); ++i)
	{
		GameState* state = new GameState();
		gameStates[i] = state;
	}
}

void Game::UpdateLobbyUI(int _amount)
{
	GameObject * lobby = scenes[currentScene]->GetUIByName("Players");
	Button * button = lobby->GetComponent<Button>();
	button->setText(to_wstring(_amount) + L"/4");
}

// updates the lobby screen
// since the networking is different on the lobby, this keeps it from sending packages when it doesn't need to
int Game::UpdateLobby()
{
	//set client id
	Game::clientID = client.getID();

	//run server
	if (ResourceManager::GetSingleton()->IsServer())
	{
		int serverState = server.run();
	}

	//run client
	int clientState = client.run();

	if (clientState == 5)
		UpdateLobbyUI(client.getNumClients());
	else if (clientState == 6)
		LoadScene("FirstLevel");
	else if (clientState == 7)
	{
		objID = client.getObjID();
		clientID = client.getID();
	}


	return clientState;
}