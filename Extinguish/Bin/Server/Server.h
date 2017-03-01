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

#define MAX_PLAYERS 4

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
		int scoreA;
		int scoreB;
		float time;


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
		ID_CLIENT_OBJ
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
	InputEventStruct clientInput[4];
	//static GAME_STATE * gameState;
	//GAME_STATE gameState;
	//static CLIENT_GAME_STATE * aiStates;
	UINT8 numPlayers = 0;
	RakPeerInterface * peer;
	Packet * packet;
	char * names[MAX_PLAYERS];
	UINT8  nameSizes[MAX_PLAYERS];
	UINT8  clientIDs[MAX_PLAYERS];
	//UINT8  bteamIDs[MAX_PLAYERS];
	OBJID objIDs[8];
	bool newInput[MAX_PLAYERS];
	std::vector<Server::CLIENT_GAME_STATE> * clientStates;
	std::vector<Server::GAME_STATE> * gameState;
	SOCKET serverSocket;
	bool npDec = false;
	char* message;
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

	/* setters */
	void setStateSize(unsigned int size) { clientStates[0].clear(); clientStates[0].resize(size);}
	void setScores(int scoreA, int scoreB) { gameState[0][0].scoreA = scoreA; gameState[0][0].scoreB = scoreB; }
	void setTime(float time) { gameState[0][0].time = time; }
	void setStates(unsigned int index, bool hasBall, XMFLOAT3 pos, XMFLOAT3 rot, int parentIndex, int animIndex, int oIndex, int transitionIndex, unsigned int soundID, bool hasSound);
	void setObjectCount(int count);
	void setObjIDs(UINT8 one, UINT8 two, UINT8 three, UINT8 four, UINT8 five, UINT8 six, UINT8 seven, UINT8 eight);


	/* game stuff*/
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
	void sendObjID(UINT8 id);
	void unregisterClient();
	void sendDisconnect(bool broadcast);
	void recievePacket();
	void recieveInput();
	void ReceiveMessage();
};

