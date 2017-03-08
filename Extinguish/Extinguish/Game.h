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
	enum PLAYER_TEAM
	{
		TEAM_A = 0,
		TEAM_B = 4
	};

	static ClientWrapper client;
	static ServerWrapper server;
	static unsigned int currentScene;
	static int returnResult;
	static PLAYER_TEAM team;
	static UINT8 objIDs[10];
	static float Time;

	int Team1Score;
	int Team2Score;
private:
	std::vector<Scene*> scenes;
	HashString scenesNamesTable;

	DeviceResources* devResources;

	//InputManager* input;
	ResourceManager* resourceManager;
	SoundEngine* soundEngine;

	std::vector<GameState*> gameStates;

	static int clientID;
	static int objID;

	//private helper functions
	void CreateScenes(InputManager* inputManager);
	void ReceiveServerMessage();
	void ResetPlayers();

	/* to separate all the ui */
	void CreateUI(Scene * basic);
	void CreateMenu(Scene * scene);
	void CreateLobby(Scene * scene);
	void CreatePauseMenu(Scene * scene);
	void CreateGame(Scene * basic, XMFLOAT4X4 identity, XMFLOAT4X4 projection);
	void AssignPlayers();

	void UpdateServerStates();
	void UpdateClientObjects(float dt);
	void UpdateScoreUI();
	void UpdateLobbyUI(int _amount);

	void LoadScene(std::string name);
	int UpdateLobby();
	void EnableButton(std::string name, bool toggle);

public:
	void CreateGameWrapper();
	~Game();
	//basic
	void Init(DeviceResources* _devResources, InputManager* inputManager);
	int Update(float dt);
	void FixedUpdate(float dt);
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