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
	//cache
	soundEngine = SoundEngine::GetSingleton();

	//register to event dispatcher
	EventDispatcher::GetSingleton()->RegisterHandler(this);

	//initialize resource manager
	resourceManager.Init(devResources);

	if (isMultiplayer)
	{
		//if (server.init("127.0.0.1", 60000) == 1)
		//{
		//	isMultiplayer = true;
		//	isServer = true;
		//
		//	client.init("127.0.0.1", 60001);
		//}
		//else
		{
			isServer = false;
			client.init("127.0.0.1", 60001);
		}
		
	}

	currentScene = 0;

	//create scenes
	CreateScenes(devResources, inputManager);

	//set up server stuff
	std::vector<GameObject*>* gameObjects = scenes[currentScene]->GetGameObjects();

	gameStates.resize(scenes[currentScene]->GetNumObjects());
	for (int i = 0; i < gameObjects->size(); ++i)
	{
		GameState* state = new GameState();
		gameStates[i] = state;
	}

	if (isServer)
	{
		server.setObjCount(scenes[currentScene]->GetNumObjects());
	}
	
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

void Game::Update(float dt)
{
	if (isMultiplayer)
	{
		//set client id
		Game::clientID = client.getID();

		// get current game states
		std::vector<GameObject*>* gameObjects = scenes[currentScene]->GetGameObjects();

		// if server, set game states
		if (isServer)
		{
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
				Renderer* renderer = gameObject->GetComponent<Renderer>();

				if (renderer)
				{
					Blender* blender = renderer->GetBlender();

					if (blender)
					{
						if (blender->GetNextInterpolator()->HasAnimation())
						{
							animIndex = blender->GetAnimationSet()->GetAnimationIndex(blender->GetNextInterpolator()->GetAnimation()->GetAnimationName());
						}
					}
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

		//if (client.getID() > 0)
		//{
		//	// get camera position
		//	client.setLocation(gameStates[clientID]->position);
		//	client.setRotation(gameStates[clientID]->rotation);

		//	// send to server
		//	client.sendPacket();
		//}

		//run server
		if (isServer)
		{
			int serverState = server.run();

			//if (serverState == 2)
			//{
			//	gameStates = server.getStates();
			//}
		}

		//run client
		int clientState = client.run();
		
		// if client gets server's game states, get the state's location from the client
		// so that it can be included in update
		if (clientState == 2 && client.getID() > 0)
		{
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
						if (parentIndex != -1)
						{
							gameObject->GetTransform()->SetParent((*gameObjects)[parentIndex]->GetTransform());
						}

						INT8 animIndex = client.GetAnimationIndex(i);

						if (animIndex != -1)
						{
							Renderer* renderer = gameObject->GetComponent<Renderer>();

							if (renderer)
							{
								Blender* blender = renderer->GetBlender();

								if (blender)
								{
									if (animIndex == 1)
									{
										int breakpoint = 0;
										breakpoint += 69;
									}

									//if (!blender->GetNextInterpolator()->HasAnimation())
									if ((blender->GetCurInterpolator()->GetAnimation()->GetAnimationName() != blender->GetAnimationSet()->GetAnimation(animIndex)->GetAnimationName()))
									{
										BlendInfo info;
										info.totalBlendTime = 0.01f;

										renderer->SetNextAnimation(animIndex);
										renderer->SetBlendInfo(info);
									}
								}
							}
						}
					}
				}
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

	//used for hexagon floor
	int row = 80; // * 2 = z
	int col = 80; // * 2 = x

	unsigned int* colors = new unsigned int[row * col];

	GameObject* mage1 = new GameObject();
	basic->AddGameObject(mage1);
	mage1->Init("Mage1");
	mage1->InitTransform(identity, { (float)-col, 0, -12 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* mageRenderer1 = new Renderer();
	mage1->AddComponent(mageRenderer1);
	mageRenderer1->Init("Mage", "NormalMapped", "Bind", "", "Idle", projection, &resourceManager, devResources);
	Movement* mageMover = new Movement();
	mage1->AddComponent(mageMover);
	mageMover->Init(5.0f, 0.75f);
	//PlayerController* bplayerController = new PlayerController();
	//mage->AddComponent(bplayerController);
	//bplayerController->Init(5.0f, 0.75f);

	GameObject* mage2 = new GameObject();
	basic->AddGameObject(mage2);
	mage2->Init("Mage2");
	mage2->InitTransform(identity, { (float)-col, 0, -4 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* mageRenderer2 = new Renderer();
	mage2->AddComponent(mageRenderer2);
	mageRenderer2->Init("Mage", "NormalMapped", "Bind", "", "Idle", projection, &resourceManager, devResources);
	Movement* mageMover2 = new Movement();
	mage2->AddComponent(mageMover2);
	mageMover2->Init(5.0f, 0.75f);

	GameObject* mage3 = new GameObject();
	basic->AddGameObject(mage3);
	mage3->Init("Mage3");
	mage3->InitTransform(identity, { (float)-col, 0, 4 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* mageRenderer3 = new Renderer();
	mage3->AddComponent(mageRenderer3);
	mageRenderer3->Init("Mage", "NormalMapped", "Bind", "", "Idle", projection, &resourceManager, devResources);
	Movement* mageMover3 = new Movement();
	mage3->AddComponent(mageMover3);
	mageMover3->Init(5.0f, 0.75f);

	GameObject* mage4 = new GameObject();
	basic->AddGameObject(mage4);
	mage4->Init("Mage4");
	mage4->InitTransform(identity, { (float)-col, 0, 12 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* mageRenderer4 = new Renderer();
	mage4->AddComponent(mageRenderer4);
	mageRenderer4->Init("Mage", "NormalMapped", "Bind", "", "Idle", projection, &resourceManager, devResources);
	Movement* mageMover4 = new Movement();
	mage4->AddComponent(mageMover4);
	mageMover4->Init(5.0f, 0.75f);

	GameObject* mage5 = new GameObject();
	basic->AddGameObject(mage5);
	mage5->Init("Mage5");
	mage5->InitTransform(identity, { (float)col, 0, -12 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* mageRenderer5 = new Renderer();
	mage5->AddComponent(mageRenderer5);
	mageRenderer5->Init("Mage", "NormalMapped", "Bind", "", "Idle", projection, &resourceManager, devResources);
	Movement* mageMover5 = new Movement();
	mage5->AddComponent(mageMover5);
	mageMover5->Init(5.0f, 0.75f);

	GameObject* mage6 = new GameObject();
	basic->AddGameObject(mage6);
	mage6->Init("Mage6");
	mage6->InitTransform(identity, { (float)col, 0, -4 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* mageRenderer6 = new Renderer();
	mage6->AddComponent(mageRenderer6);
	mageRenderer6->Init("Mage", "NormalMapped", "Bind", "", "Idle", projection, &resourceManager, devResources);
	Movement* mageMover6 = new Movement();
	mage6->AddComponent(mageMover6);
	mageMover6->Init(5.0f, 0.75f);

	GameObject* mage7 = new GameObject();
	basic->AddGameObject(mage7);
	mage7->Init("Mage7");
	mage7->InitTransform(identity, { (float)col, 0, 4 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* mageRenderer7 = new Renderer();
	mage7->AddComponent(mageRenderer7);
	mageRenderer7->Init("Mage", "NormalMapped", "Bind", "", "Idle", projection, &resourceManager, devResources);
	Movement* mageMover7 = new Movement();
	mage7->AddComponent(mageMover7);
	mageMover7->Init(5.0f, 0.75f);

	GameObject* mage8 = new GameObject();
	basic->AddGameObject(mage8);
	mage8->Init("Mage8");
	mage8->InitTransform(identity, { (float)col, 0, 12 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* mageRenderer8 = new Renderer();
	mage8->AddComponent(mageRenderer8);
	mageRenderer8->Init("Mage", "NormalMapped", "Bind", "", "Idle", projection, &resourceManager, devResources);
	Movement* mageMover8 = new Movement();
	mage8->AddComponent(mageMover8);
	mageMover8->Init(5.0f, 0.75f);

	GameObject* camera1 = new GameObject();
	basic->AddGameObject(camera1);
	camera1->Init("Camera1");
	camera1->InitTransform(identity, { 0, 0, -1.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage1->GetTransform(), nullptr, nullptr);
	Camera* cameraController1 = new Camera();
	camera1->AddComponent(cameraController1);
	cameraController1->Init({ 0.0f, 0.7f, 1.5f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

	GameObject* camera2 = new GameObject();
	basic->AddGameObject(camera2);
	camera2->Init("Camera2");
	camera2->InitTransform(identity, { 0, 0, -1.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage2->GetTransform(), nullptr, nullptr);
	Camera* cameraController2 = new Camera();
	camera2->AddComponent(cameraController2);
	cameraController2->Init({ 0.0f, 0.7f, 1.5f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

	GameObject* camera3 = new GameObject();
	basic->AddGameObject(camera3);
	camera3->Init("Camera3");
	camera3->InitTransform(identity, { 0, 0, -1.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage3->GetTransform(), nullptr, nullptr);
	Camera* cameraController3 = new Camera();
	camera3->AddComponent(cameraController3);
	cameraController3->Init({ 0.0f, 0.7f, 1.5f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

	GameObject* camera4 = new GameObject();
	basic->AddGameObject(camera4);
	camera4->Init("Camera4");
	camera4->InitTransform(identity, { 0, 0, -1.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage4->GetTransform(), nullptr, nullptr);
	Camera* cameraController4 = new Camera();
	camera4->AddComponent(cameraController4);
	cameraController4->Init({ 0.0f, 0.7f, 1.5f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

	GameObject* camera5 = new GameObject();
	basic->AddGameObject(camera5);
	camera5->Init("Camera5");
	camera5->InitTransform(identity, { 0, 0, -1.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage5->GetTransform(), nullptr, nullptr);
	Camera* cameraController5 = new Camera();
	camera5->AddComponent(cameraController5);
	cameraController5->Init({ 0.0f, 0.7f, 1.5f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

	GameObject* camera6 = new GameObject();
	basic->AddGameObject(camera6);
	camera6->Init("Camera6");
	camera6->InitTransform(identity, { 0, 0, -1.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage6->GetTransform(), nullptr, nullptr);
	Camera* cameraController6 = new Camera();
	camera6->AddComponent(cameraController6);
	cameraController6->Init({ 0.0f, 0.7f, 1.5f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

	GameObject* camera7 = new GameObject();
	basic->AddGameObject(camera7);
	camera7->Init("Camera7");
	camera7->InitTransform(identity, { 0, 0, -1.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage7->GetTransform(), nullptr, nullptr);
	Camera* cameraController7 = new Camera();
	camera7->AddComponent(cameraController7);
	cameraController7->Init({ 0.0f, 0.7f, 1.5f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

	GameObject* camera8 = new GameObject();
	basic->AddGameObject(camera8);
	camera8->Init("Camera8");
	camera8->InitTransform(identity, { 0, 0, -1.6f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage8->GetTransform(), nullptr, nullptr);
	Camera* cameraController8 = new Camera();
	camera8->AddComponent(cameraController8);
	cameraController8->Init({ 0.0f, 0.7f, 1.5f, 0.0f }, { 0.0f, 0.1f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f }, 5.0f, 0.75f);

	GameObject* meterbox6 = new GameObject();
	basic->AddGameObject(meterbox6);
	meterbox6->Init("MeterBox6");
	meterbox6->InitTransform(identity, { 0,0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* meterboxRenderer6 = new Renderer();
	meterbox6->AddComponent(meterboxRenderer6);
	meterboxRenderer6->Init("MeterBox", "Static", "Static", "", "", projection, &resourceManager, devResources);
	BoxCollider* meterboxcol6 = new BoxCollider(meterbox6, false, { 300,0.5f,300 }, { -300,-0.5f,-300 });
	meterbox6->AddBoxCollider(meterboxcol6);
	float3* floor = CreateFloor(2.0f, row, col, float3(-row, -10, -col));

	GameObject* HexFloor = new GameObject();
	basic->AddGameObject(HexFloor);
	HexFloor->Init("HexFloor");
	HexFloor->InitTransform(identity, { 0,0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }, nullptr, nullptr, nullptr);
	Renderer* HexFloorRenderer = new Renderer();
	HexFloor->AddComponent(HexFloorRenderer);
	HexFloorRenderer->Init(row * col, floor, colors,"Hexagon", "InstStatic", "InstancedStatic", "", "", projection, &resourceManager, devResources);
	HexagonCollider* HexFLoorCol = new HexagonCollider( row, col, floor, 10, 2,HexFloor);
	HexFloor->AddComponent(HexFLoorCol);
	FloorController* fcon = new FloorController(floor, row, col, 10, colors);
	HexFloor->AddComponent(fcon);

	GameObject* Hex = new GameObject();
	basic->AddGameObject(Hex);
	Hex->Init("Team2");
	Hex->InitTransform(identity, { 1,0,-5 }, { 0,0,0 }, { 1,1,1 }, nullptr, nullptr, nullptr);
	Renderer* ballrenderer3 = new Renderer();
	Hex->AddComponent(ballrenderer3);
	ballrenderer3->Init("Hexagon", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Movement* ballMover3 = new Movement();
	//Hex->AddComponent(ballMover3);
	//ballMover3->Init(5.0f, 0.75f);
	//HexagonCollider* ballcol3 = new HexagonCollider(Hex,2,10);
	//Hex->AddComponent(ballcol3);

	//Hex->SetTag("Goal2");
	


	//GameObject* bear = new GameObject();
	//basic->AddGameObject(bear);
	//bear->Init("Bear");
	//bear->InitTransform(identity, { 5, 0, 0 }, { 0, -180, 0 }, { 0.01f, 0.01f, 0.01f }, nullptr, nullptr, nullptr);
	//Renderer* bearRenderer = new Renderer();
	//bear->AddComponent(bearRenderer);
	//bearRenderer->Init("Teddy", "NormalMapped", "Bind", "", "Idle", projection, &resourceManager, devResources);
	//BlendInfo bearBI;
	//bearBI.totalBlendTime = 0.01f;
	//bearRenderer->SetBlendInfo(bearBI);
	//bearRenderer->SetNextAnimation("Run");
	////bearRenderer->SetNextAnimation("Run");
	////PlayerController* playerController = new PlayerController();
	////bear->AddComponent(playerController);
	////playerController->Init();
	//Movement* bearMover = new Movement();
	//bear->AddComponent(bearMover);
	//bearMover->Init(1.0f, 0.75f);
	//CapsuleCollider* bearcol = new CapsuleCollider(0.5, { 0,0,0 }, { 0, 1, 0 }, bear, false); // bear, false, { 1,2,1 }, { -1,0,-1 }
	//bear->AddCapsuleCollider(bearcol);
	//
	//bear->SetTag("Team1");
	//AI *bearAI = new AI(bear);
	//bear->AddComponent(bearAI);

	GameObject* gameBall = new GameObject();
	basic->AddGameObject(gameBall);
	GameObject* crosse = new GameObject();
	basic->AddGameObject(crosse);

	gameBall->Init("GameBall");
	gameBall->InitTransform(identity, { 0, 0, 0.1f }, { 0, 0, 0 }, { 0.2f, 0.2f, 0.2f }, crosse->GetTransform(), nullptr, nullptr);
	Renderer* gameBallRenderer = new Renderer();
	gameBall->AddComponent(gameBallRenderer);
	gameBallRenderer->Init("Ball", "Static", "Static", "", "", projection, &resourceManager, devResources);
	SphereCollider* gameBallCollider = new SphereCollider(0.125f, gameBall, false);
	gameBall->AddSphereCollider(gameBallCollider);
	BallController* ballController = new BallController(gameBall);
	gameBall->AddComponent(ballController);
	ballController->Init();
	ballController->SetHolder(crosse);

	gameBall->SetTag("Ball");
	//bearAI->Init();


	crosse->Init("Crosse1");
	crosse->InitTransform(identity, { 0, 5.4f, -1.7f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage1->GetTransform(), nullptr, nullptr);
	SphereCollider* crosseNetCollider = new SphereCollider(0.25f, crosse, true);
	crosse->AddSphereCollider(crosseNetCollider);
	Renderer* crosseRenderer = new Renderer();
	crosse->AddComponent(crosseRenderer);
	crosseRenderer->Init("Crosse", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Crosse* crosseController = new Crosse();
	crosse->AddComponent(crosseController);
	crosseController->Init();

	GameObject* crosse2 = new GameObject();
	basic->AddGameObject(crosse2);
	crosse2->Init("Crosse2");
	crosse2->InitTransform(identity, { 0, 5.4f, -1.7f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage2->GetTransform(), nullptr, nullptr);
	//crosse->InitTransform(identity, { 0.5f, 0.15f, 0.9f }, { 0, 1 * XM_PI, -0.25f * XM_PI }, { 0.001f, 0.001f, 0.001f }, camera->GetTransform(), nullptr, nullptr);
	SphereCollider* crosseNetCollider2 = new SphereCollider(0.25f, crosse2, true);
	crosse2->AddSphereCollider(crosseNetCollider2);
	Renderer* crosseRenderer2 = new Renderer();
	crosse2->AddComponent(crosseRenderer2);
	crosseRenderer2->Init("Crosse", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Crosse* crosseController2 = new Crosse();
	crosse2->AddComponent(crosseController2);
	crosseController2->Init();

	GameObject* crosse3 = new GameObject();
	basic->AddGameObject(crosse3);
	crosse3->Init("Crosse3");
	crosse3->InitTransform(identity, { 0, 5.4f, -1.7f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage3->GetTransform(), nullptr, nullptr);
	//crosse->InitTransform(identity, { 0.5f, 0.15f, 0.9f }, { 0, 1 * XM_PI, -0.25f * XM_PI }, { 0.001f, 0.001f, 0.001f }, camera->GetTransform(), nullptr, nullptr);
	SphereCollider* crosseNetCollider3 = new SphereCollider(0.25f, crosse2, true);
	crosse3->AddSphereCollider(crosseNetCollider3);
	Renderer* crosseRenderer3 = new Renderer();
	crosse3->AddComponent(crosseRenderer3);
	crosseRenderer3->Init("Crosse", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Crosse* crosseController3 = new Crosse();
	crosse3->AddComponent(crosseController3);
	crosseController3->Init();

	GameObject* crosse4 = new GameObject();
	basic->AddGameObject(crosse4);
	crosse4->Init("Crosse4");
	crosse4->InitTransform(identity, { 0, 5.4f, -1.7f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage4->GetTransform(), nullptr, nullptr);
	//crosse->InitTransform(identity, { 0.5f, 0.15f, 0.9f }, { 0, 1 * XM_PI, -0.25f * XM_PI }, { 0.001f, 0.001f, 0.001f }, camera->GetTransform(), nullptr, nullptr);
	SphereCollider* crosseNetCollider4 = new SphereCollider(0.25f, crosse2, true);
	crosse4->AddSphereCollider(crosseNetCollider4);
	Renderer* crosseRenderer4 = new Renderer();
	crosse4->AddComponent(crosseRenderer4);
	crosseRenderer4->Init("Crosse", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Crosse* crosseController4 = new Crosse();
	crosse4->AddComponent(crosseController4);
	crosseController4->Init();

	GameObject* crosse5 = new GameObject();
	basic->AddGameObject(crosse5);
	crosse5->Init("Crosse5");
	crosse5->InitTransform(identity, { 0, 5.4f, -1.7f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage5->GetTransform(), nullptr, nullptr);
	//crosse->InitTransform(identity, { 0.5f, 0.15f, 0.9f }, { 0, 1 * XM_PI, -0.25f * XM_PI }, { 0.001f, 0.001f, 0.001f }, camera->GetTransform(), nullptr, nullptr);
	SphereCollider* crosseNetCollider5 = new SphereCollider(0.25f, crosse2, true);
	crosse5->AddSphereCollider(crosseNetCollider5);
	Renderer* crosseRenderer5 = new Renderer();
	crosse5->AddComponent(crosseRenderer5);
	crosseRenderer5->Init("Crosse", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Crosse* crosseController5 = new Crosse();
	crosse5->AddComponent(crosseController5);
	crosseController5->Init();

	GameObject* crosse6 = new GameObject();
	basic->AddGameObject(crosse6);
	crosse6->Init("Crosse6");
	crosse6->InitTransform(identity, { 0, 5.4f, -1.7f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage6->GetTransform(), nullptr, nullptr);
	//crosse->InitTransform(identity, { 0.5f, 0.15f, 0.9f }, { 0, 1 * XM_PI, -0.25f * XM_PI }, { 0.001f, 0.001f, 0.001f }, camera->GetTransform(), nullptr, nullptr);
	SphereCollider* crosseNetCollider6 = new SphereCollider(0.25f, crosse2, true);
	crosse6->AddSphereCollider(crosseNetCollider6);
	Renderer* crosseRenderer6 = new Renderer();
	crosse6->AddComponent(crosseRenderer6);
	crosseRenderer6->Init("Crosse", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Crosse* crosseController6 = new Crosse();
	crosse6->AddComponent(crosseController6);
	crosseController6->Init();

	GameObject* crosse7 = new GameObject();
	basic->AddGameObject(crosse7);
	crosse7->Init("Crosse7");
	crosse7->InitTransform(identity, { 0, 5.4f, -1.7f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage7->GetTransform(), nullptr, nullptr);
	//crosse->InitTransform(identity, { 0.5f, 0.15f, 0.9f }, { 0, 1 * XM_PI, -0.25f * XM_PI }, { 0.001f, 0.001f, 0.001f }, camera->GetTransform(), nullptr, nullptr);
	SphereCollider* crosseNetCollider7 = new SphereCollider(0.25f, crosse2, true);
	crosse7->AddSphereCollider(crosseNetCollider7);
	Renderer* crosseRenderer7 = new Renderer();
	crosse7->AddComponent(crosseRenderer7);
	crosseRenderer7->Init("Crosse", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Crosse* crosseController7 = new Crosse();
	crosse7->AddComponent(crosseController7);
	crosseController7->Init();

	GameObject* crosse8 = new GameObject();
	basic->AddGameObject(crosse8);
	crosse8->Init("Crosse8");
	crosse8->InitTransform(identity, { 0, 5.4f, -1.7f }, { 0, XM_PI, 0 }, { 1, 1, 1 }, mage8->GetTransform(), nullptr, nullptr);
	//crosse->InitTransform(identity, { 0.5f, 0.15f, 0.9f }, { 0, 1 * XM_PI, -0.25f * XM_PI }, { 0.001f, 0.001f, 0.001f }, camera->GetTransform(), nullptr, nullptr);
	SphereCollider* crosseNetCollider8 = new SphereCollider(0.25f, crosse2, true);
	crosse8->AddSphereCollider(crosseNetCollider8);
	Renderer* crosseRenderer8 = new Renderer();
	crosse8->AddComponent(crosseRenderer8);
	crosseRenderer8->Init("Crosse", "Static", "Static", "", "", projection, &resourceManager, devResources);
	Crosse* crosseController8 = new Crosse();
	crosse8->AddComponent(crosseController8);
	crosseController8->Init();

	scenes.push_back(basic);
	scenesNamesTable.Insert("FirstLevel");
}

//getters
int Game::GetClientID()
{
	return clientID;
}