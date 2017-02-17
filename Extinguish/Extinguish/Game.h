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

	int Team1Score;
	int Team2Score;
private:
	std::vector<Scene*> scenes;
	HashString scenesNamesTable;

	//InputManager* input;
	ResourceManager* resourceManager;
	SoundEngine* soundEngine;

	DeviceResources* Dresources;

	std::vector<GameState*> gameStates;

	static int clientID;

	//private helper functions
	void CreateScenes(DeviceResources* devResources, InputManager* inputManager);
	void CreateUI(DeviceResources * devResources, Scene * basic);

	void CreateMenu(DeviceResources * devResources, Scene * scene);
	void CreateLobby(DeviceResources * devResources, Scene * scene);

	void UpdateServerStates();
	void UpdateClientObjects();
	void UpdateScoreUI();
	void UpdateLobbyUI(int _amount);

	void LoadScene(std::string name);
	int UpdateLobby();
public:

	//basic
	void Init(DeviceResources* devResources, InputManager* inputManager);
	void Update(float dt);
	void Render();
	void Shutdown();

	//misc
	//void LoadScene(unsigned int index);
	void HandleEvent(Event* e);
	void WindowResize(uint16_t w, uint16_t h);

	//getters
	static int GetClientID();
	static int GetPlayerObjectID();
};