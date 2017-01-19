#pragma once

//#include <WinSock2.h>
//#include <Windows.h>
#include "Includes.h"
#include <vector>
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
#include "Client Wrapper.h"
#include "Server Wrapper.h"

class Game
{
private:
	std::vector<Scene*> scenes;
	unsigned int currentScene;
	HashString scenesNamesTable;

	//InputManager* input;
	ResourceManager resourceManager;

	bool isServer = true;
	bool isMultiplayer = true;

	ClientWrapper client;
	ServerWrapper server;

	//private helper functions
	void CreateScenes(DeviceResources* devResources, InputManager* inputManager);
public:
	void Init(DeviceResources* devResources, InputManager* inputManager);
	void Update(float dt);
	void Render();
	void Shutdown();
	void LoadScene(unsigned int index);
};