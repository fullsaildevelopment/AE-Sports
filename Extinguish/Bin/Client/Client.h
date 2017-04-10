#pragma once

//#define _WINSOCKAPI_


#ifdef CLIENTDLL_EXPORTS
#define CLIENTDLL_API __declspec(dllexport)
#else
#define CLIENTDLL_API __declspec(dllimport)
#endif

//#include "..\RakNet\WindowsIncludes.h"
#include <stdio.h>
#include <string.h>
#include <string>
//#include <iostream>

#include "..\RakNet\Gets.h"
#include "..\RakNet\RakPeerInterface.h"
#include "..\RakNet\MessageIdentifiers.h"
#include "..\RakNet\BitStream.h"
#include "..\RakNet\RakNetTypes.h"
#include "..\RakNet\Router2.h"
#include "..\RakNet\Kbhit.h"
#include "..\RakNet\GetTime.h"

#include <DirectXMath.h>
#include <vector>


using namespace RakNet;
using namespace std;
using namespace DirectX;
#define MAX_PLAYERS 8

class CLIENTDLL_API Client
{
public:

#pragma pack(push, 1)
	struct CLIENT_GAME_STATE
	{
		UINT8 clientID;
		UINT8 nameLength;
		//char animationName[125];
		bool hasBall;
		//UINT8 hasBall;
		//	XMFLOAT4X4 world;
		XMFLOAT3 position;
		XMFLOAT3 rotation;
		INT8 parentIndex;
		INT8 animationIndex;
		int otherIndex;
		float _dt;
		INT8 transitionIndex;
		UINT32 soundID;
		bool hasSound;

		CLIENT_GAME_STATE() {}
	};
#pragma pack(pop)


#pragma pack(push, 1)
	struct GAME_STATE
	{
		bool paused = true;
		bool countdown = false;
		UINT8 scene = 1;
		int floorPulse = -1;
		int scoreA = 0;
		int scoreB = 0;
		float time = 0.0f;
		float _dt = 0.0f;
		bool sprintA = false;
		bool sprintD = true;
		bool empty = false;
		bool down = false;
		UINT8 score = 0;
		UINT8 assists = 0;
		UINT8 saves = 0;
		UINT8 goals = 0;
		char* name = nullptr;

		GAME_STATE() {}
	};
#pragma pack(pop)

private:

	enum GameMessages
	{
		ID_CLIENT_MESSAGE = ID_USER_PACKET_ENUM + 2,
		ID_SERVER_MESSAGE,
		ID_CLIENT_DISCONNECT,
		ID_SERVER_CLOSURE,
		ID_CLIENT_REGISTER,
		ID_REQUEST,
		ID_INCOMING_PACKET,
		ID_REMOVE_CLIENT,
		ID_INCOMING_INPUT,
		ID_INCOMING_STATE,
		ID_NEW_CLIENT,
		ID_START_GAME,
		ID_INCOMING_MESSAGE,
		ID_CHANGE_TEAM_A,
		ID_CHANGE_TEAM_B,
		ID_CLIENT_OBJ,
		ID_SPRINT_EMPTY,
		ID_SOMEONE_SCORED,
		ID_SPAWN_POWERUP,
		ID_REMOVE_POWERUP,
		ID_POWERUP_TIME

	};


#pragma pack(push, 1)
	struct PowerUps
	{
		// spawn
		std::vector<bool> isactive;
		std::vector<XMFLOAT3> positions;
		// despawn
		std::vector<int> removeindices;
		std::vector<int> ids;
		// elapsed time
		std::vector<float> elapsedTime;
	};
#pragma pack(pop)

	static RakPeerInterface * peer;
	static Packet * packet;
	static char * address;
	std::vector<CLIENT_GAME_STATE> * clientStates;
	std::vector<CLIENT_GAME_STATE> * myState;
	std::vector<GAME_STATE> * gameState;

	UINT8 curNumOfClients;
	UINT8 objID;

	PowerUps pUp;

	UINT8 objects;
	int numPackets = 0;
	bool disconnect = false;
	UINT8 clientID;
//	char clientName[8];

	char* message;
	uint16_t messageID, stride;

	string * scoreName;

	// all the results
	bool states, scored, packets, spowerup, rpowerup;

public:

#pragma pack(push, 1)
	struct InputEventStruct
	{
		bool keyboard[256];
		bool keyboardDown[256];
		bool keyboardUp[256];
		bool mouse[3];
		bool mouseDown[3];
		bool mouseUp[3];
		UINT16 mouseX, mouseY;
		UINT8 clientID;
		bool isServer;
	};

#pragma pack(pop)
	Client();
	~Client();

	int init(char* address, UINT16 port);
	int run();
	void stop();

	// send functions
	int sendInput(bool keyboard[256], bool keyboardDown[256], bool keyboardUp[256], bool mouse[3], bool mouseDown[3], bool mouseUp[3], int mouseX, int mouseY, bool isServer);
	void sendStop();
	void sendMessage(char * newMessage);
	void sendMessage(char * message, uint16_t stride);
	void sendPacket();
	void changeTeam(UINT16 team);
	void sendEmpty(bool empty);

	//receive functions
	void ReceiveMessage();

	// setters
	void setLocation(XMFLOAT3 loc) { myState[0][0].position = loc; }
	void setRotation(XMFLOAT3 rot) { myState[0][0].rotation = rot; }

	// getters
	int getFloorState(unsigned int i) { return clientStates[0][i].otherIndex; }
	UINT8 getScoreA() { return gameState[0][0].scoreA; }
	UINT8 getScoreB() { return gameState[0][0].scoreB; }
	float getTime() { return gameState[0][0].time; }
	bool getMeterActive() { 
		if (clientID == 0)
			return false;
		return gameState[0][clientID - 1].sprintA;
	}
	bool getMeterDrain() {
		if (clientID == 0)
			return false; 
		return gameState[0][clientID - 1].sprintD; }
	bool getMeterEmpty() {
		if (clientID == 0)
			return false; 
		return gameState[0][clientID - 1].empty; }
	bool getMeterDown() {
		if (clientID == 0)
			return false; 
		return gameState[0][clientID - 1].down; }
	UINT8 getNumClients() { return curNumOfClients; }
	XMFLOAT3 getLocation(unsigned int index) { return clientStates[0][index].position; }
	XMFLOAT3 getRotation(unsigned int index) { return clientStates[0][index].rotation; }
	int getNumPackets() { return numPackets; }
	INT8 GetParentIndex(unsigned int index) { return clientStates[0][index].parentIndex; }
	INT8 GetAnimationIndex(unsigned int index) { return clientStates[0][index].animationIndex; }
	INT8 GetTransitionIndex(unsigned int index) { return clientStates[0][index].transitionIndex; }
	UINT32 GetSoundID(unsigned int index) { return clientStates[0][index].soundID; }
	UINT8 getID() { return clientID; }
	CLIENT_GAME_STATE getMyState();
	bool getPaused() { return gameState[0][0].paused; }
	UINT8 getScene() { return gameState[0][0].scene; }
	//UINT8 hasBall(unsigned int index) { return clientStates[index].hasBall; }
	bool hasBall(unsigned int index) { return clientStates[0][index].hasBall; }
	bool HasSound(unsigned int index) { return clientStates[0][index].hasSound; }
	UINT8 getObjID() { return objID; }
	char* getScoreboard(unsigned int i, unsigned int &score, unsigned int &assists, unsigned int &saves, unsigned int &goals, char * name)
	{
		//name = gameState[0][i].name;
		name = new char[strlen(gameState[0][i].name) + 1];
		strcpy(name, gameState[0][i].name);
		score = gameState[0][i].score;
		assists = gameState[0][i].assists;
		saves = gameState[0][i].saves;
		goals = gameState[0][i].goals;

		return name;
	}

	int stateSize() { return (int)clientStates[0].size(); }
	float getDT() { return gameState[0][0]._dt; }
	bool getCountdown() { return gameState[0][0].countdown; }
	char* getMessage() { return message; }
	uint16_t GetMessageID() { return messageID; }
	uint16_t GetStride() { return stride; }
	const char * getScoreName() { return scoreName[0].c_str(); }
	int getPulse() { return gameState[0][0].floorPulse; }

	// power ups
	int SpawnedPowerUpAmount() { return 6; }
	bool getSpawnedPowerUpActive(int i) { return pUp.isactive[i]; }
	XMFLOAT3 GetSpawnedPowerUpPos(int i) { return pUp.positions[i]; }
	int RemovedPowerUpAmount() { return (int)pUp.removeindices.size(); }
	int getRemovedPowerUpIndex(int i) { return pUp.removeindices[i]; }
	int getRemovedPlayerID(int i) { return pUp.ids[i]; }
	float getPowerUpTime(int i) { return pUp.elapsedTime[i]; }

	// which packets have arrived
	bool hasScored() { return scored; }
	bool hasPackets() { return packets; }
	bool hasState() { return states; }
	bool SpawnedPowerUps() { return spowerup; }
	bool RemovedPowerUp() { return rpowerup; }

private:
	bool gameStart = false;
	void sendMessage(char * message, GameMessages ID);
	void sendMessage(char * message, GameMessages ID, SystemAddress sAddress);
	void sendMessage(UINT8 clientid, GameMessages ID, SystemAddress sAddress);
	void readMessage();
	void GetNewID();
	void registerName();
	void receivePackets();
	void receiveGameState();
	void receiveRPU();
	void receiveSPU();
	void receiveEPU();
};

