#pragma once

//#include <WinSock2.h>
//#include <Windows.h>
#include <vector>
#include <string>
#include "Includes.h"
#include "Scene.h"
#include "InputManager.h"
#include "HashString.h"
#include "Server Wrapper.h"
#include "Client Wrapper.h"

class BallController;
class Button;
class UIRenderer;
class FloorController;
class HexagonCollider;
class BoxCollider;
class CapsuleCollider;
class SphereCollider;
class Camera;
class Crosse;
class Transform;
class PlayerConroller;
class SoundEngine;

class Game : public EventHandler
{
public:
	static ClientWrapper client;
	static ServerWrapper server;
	static unsigned int currentScene;
private:
	std::vector<Scene*> scenes;
	HashString scenesNamesTable;

	//InputManager* input;
	ResourceManager* resourceManager;
	SoundEngine* soundEngine;


	std::vector<GameState*> gameStates;

	static int clientID;

	//private helper functions
	void CreateScenes(DeviceResources* devResources, InputManager* inputManager);
	void CreateUI(DeviceResources * devResources, Scene * basic);

	void CreateMenu(DeviceResources * devResources, Scene * scene);
	void CreateLobby(DeviceResources * devResources, Scene * scene);


	/*void StartServer();
	void JoinServer();
	static void StartServerStatic(void * obj) { static_cast<Game *>(obj)->StartServer(); }
	static void JoinServerStatic(void * obj)  { static_cast<Game *>(obj)->JoinServer(); }*/

	void UpdateServerStates();
	void UpdateClientObjects();
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