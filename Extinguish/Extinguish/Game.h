#pragma once

//#include <WinSock2.h>
//#include <Windows.h>
#include <vector>
#include <string>
#include "Includes.h"
#include "Scene.h"
#include "InputManager.h"
#include "HashString.h"
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

class BallController;

class Game : public EventHandler
{
private:
	std::vector<Scene*> scenes;
	unsigned int currentScene;
	HashString scenesNamesTable;

	//InputManager* input;
	ResourceManager resourceManager;
	SoundEngine* soundEngine;

	bool isServer = true;
	bool isMultiplayer = false;
	bool gamePaused = false;

	std::vector<GameState*> gameStates;

	static int clientID;

	//private helper functions
	void CreateScenes(DeviceResources* devResources, InputManager* inputManager);
	void CreateUI(DeviceResources * devResources, Scene * basic);
	void CreateMenu(DeviceResources * devResources, Scene * scene);
	void CreateLobby(DeviceResources * devResources, Scene * scene);


	void StartServer();
	void JoinServer();
	static void StartServerStatic(void * obj) { static_cast<Game *>(obj)->StartServer(); }
	static void JoinServerStatic(void * obj)  { static_cast<Game *>(obj)->JoinServer(); }

public:
	//basic
	void Init(DeviceResources* devResources, InputManager* inputManager);
	void Update(float dt);
	void Render();
	void Shutdown();

	//misc
	void LoadScene(unsigned int index);
	void HandleEvent(Event* e);

	//getters
	static int GetClientID();
};