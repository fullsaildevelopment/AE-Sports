#pragma once

#ifdef SERVERDLL_EXPORTS
#define SERVERDLL_API __declspec(dllexport)
#else
#define SERVERDLL_API __declspec(dllimport)
#endif

#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include "..\RakNet\RakPeerInterface.h"
#include "..\RakNet\MessageIdentifiers.h"
#include "..\RakNet\BitStream.h"
#include "..\RakNet\RakNetTypes.h"
#include "..\RakNet\Router2.h"
#include "..\RakNet\GetTime.h"
#include <DirectXMath.h>
#include <list>
#include <vector>

using namespace RakNet;
using namespace std;
using namespace DirectX;

#define MAX_PLAYERS 8

class SERVERDLL_API Server
{
public:
	
#pragma pack(push, 1)
	struct CLIENT_GAME_STATE
	{
		Time timeStamp;
		UINT8 clientID;
		UINT8 nameLength;
		//char animationName[125];
		bool hasBall;
		//UINT8 hasBall;
		//XMFLOAT4X4 world;
		XMFLOAT3 position;
		XMFLOAT3 rotation;
		INT8 parentIndex;
		INT8 animationIndex;
		int otherIndex;
		INT8 transitionIndex;
		UINT32 soundID;
		bool hasSound;

		CLIENT_GAME_STATE() {}
	};
#pragma pack(pop)



#pragma pack(push, 1)
	struct GAME_STATE
	{
		int scoreA = 0;
		int scoreB = 0;
		float time = 300.0f;
		float _dt = 0.0333f;
		bool sprintA = false;
		bool sprintD = true;
		bool down = false;
		bool empty = false;
		UINT8 score = 0;
		UINT8 assists = 0;
		UINT8 saves = 0;
		UINT8 goals = 0;
		char * name;

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
		ID_INCOMING_FLOOR
	};

#pragma pack(push, 1)
	struct KeyStates
	{
		UINT8 up;
		UINT8 down;
		UINT8 left;
		UINT8 right;
	};
#pragma pack(pop)


#pragma pack(push, 1)
	struct OBJID
	{
		UINT8 id;
		bool inUse;
	};
#pragma pack(pop)

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

	
	//static CLIENT_GAME_STATE * clientStates;
	InputEventStruct clientInput[MAX_PLAYERS];
	//static GAME_STATE * gameState;
	//GAME_STATE gameState;
	//static CLIENT_GAME_STATE * aiStates;
	UINT8 numPlayers = 0;
	RakPeerInterface * peer;
	Packet * packet;
//	char * names[MAX_PLAYERS];
//	UINT8  nameSizes[MAX_PLAYERS];
	UINT8  clientIDs[MAX_PLAYERS];
	//UINT8  bteamIDs[MAX_PLAYERS];
	OBJID objIDs[MAX_PLAYERS];
	bool newInput[MAX_PLAYERS];
	std::vector<Server::CLIENT_GAME_STATE> * clientStates;
	std::vector<Server::GAME_STATE> * gameState;
	std::vector<XMFLOAT3> * floorState;
	SOCKET serverSocket;
	bool npDec = false;

	//for any message where stride will be cheap way of message header
	char* message;
	uint16_t stride;

public:
	Server();
	~Server();

	int init(uint16_t port);
	int update();
	void stop();
	bool Shutdown();

	/* getters */
	unsigned int getPlayerCount() { return numPlayers; }
	int getNewState() { return lastState; }
	InputEventStruct * getInputEvent(unsigned int index) { return &clientInput[index]; }
	CLIENT_GAME_STATE * getState(unsigned int i) { return &clientStates[0][i]; }
	bool isInput(unsigned int index) { return newInput[index]; }
	XMFLOAT3 getLocation(unsigned int index) { return clientStates[0][index].position; }
	XMFLOAT3 getRotation(unsigned int index) { return clientStates[0][index].rotation; }
	char* getMessage() { return message; }
	uint16_t GetStride() { return stride; }
	bool isPlayer(unsigned int i) { return objIDs[i].inUse; }
	UINT8 getObjID(unsigned int i) { return objIDs[i].id; }
	bool getMeterDrain(unsigned int i) { return gameState[0][i].sprintD; }
	bool getMeterActive(unsigned int i) { return gameState[0][i].sprintA; }
	bool getEmpty(unsigned int i) { return gameState[0][i].empty; }

	/* setters */
	void setStateSize(unsigned int size) { 
		if (!peer)
			float temp = 0.0f;

		clientStates[0].clear(); 
		clientStates[0].resize(size);
	}
	void setScores(int scoreA, int scoreB) { gameState[0][0].scoreA = scoreA; gameState[0][0].scoreB = scoreB; }
	void setTime(float time, float _dt) { gameState[0][0].time = time; gameState[0][0]._dt = _dt; }
	void setStates(unsigned int index, bool hasBall, XMFLOAT3 pos, XMFLOAT3 rot, int parentIndex, int animIndex, int oIndex, float _dt, int transitionIndex, unsigned int soundID, bool hasSound);
	void setObjectCount(int count);
	void setObjIDs(UINT8 one, UINT8 two, UINT8 three, UINT8 four, UINT8 five, UINT8 six, UINT8 seven, UINT8 eight);
	void setMeterActive(bool toggle, unsigned int index) { gameState[0][index].sprintA = toggle; }
	void setMeterDrain(bool toggle, unsigned int index) { gameState[0][index].sprintD = toggle; }
	void setMeterDown(bool toggle, unsigned int index) { gameState[0][index].down = toggle; }
	void setFloorState(float one, float two, float three) {
		floorState[0].push_back({ one, two, three });
	}
	void updateScoreboard(unsigned int index, unsigned int score, unsigned int assists, unsigned int saves, unsigned int goals, char * name)
	{
		gameState[0][index].score = (UINT8)score;
		gameState[0][index].assists = (UINT8)assists;
		gameState[0][index].goals = (UINT8)goals;
		gameState[0][index].saves = (UINT8)saves;
		memcpy(gameState[0][index].name, name, strlen(name));
	}


	/* game stuff*/
	void clearFloor() { floorState[0].clear(); }
	void sendFloor();
	void sendPackets();
	void updateState(unsigned int index, XMFLOAT3 position, XMFLOAT3 rotation) { 
		clientStates[0][index].position = position;
	clientStates[0][index].rotation = rotation;
	}

	void sendState();
	void StartGame();
	void resetInput() {
		newInput[0] = false;
		newInput[1] = false;
		newInput[2] = false;
		newInput[3] = false;
	}
private:
	int lastState = 0;
	int packRec = 0;
	int serverObjs = 0;
	bool shutdown = false;
	int shutdowntimer = 0;
	void sendMessage(char * message, GameMessages ID, bool broadcast);
	void sendMessage(char * message, unsigned int length, GameMessages ID, bool broadcast);
	void sendMessage(UINT8 message, GameMessages ID, bool broadcast);
	void rerouteMessage();
	UINT16 registerClient();
	void sendNew();
	void sendObjID(UINT8 id, UINT8 newID);
	void unregisterClient();
	void sendDisconnect(bool broadcast);
	void recievePacket();
	void recieveInput();
	void ReceiveMessage();
};

