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
		char animationName[125];
		bool hasBall = false;
		//XMFLOAT4X4 world;
		XMFLOAT3 position;
		XMFLOAT3 rotation;

		CLIENT_GAME_STATE() {}
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
		ID_INCOMING_INPUT
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
	struct InputEventStruct
	{
		bool keyboard[256];
		bool keyboardDown[256];
		bool keyboardUp[256];
		bool mouse[3];
		bool mouseDown[3];
		bool mouseUp[3];
		UINT8 mouseX, mouseY;
		UINT8 clientID;
	};

#pragma pack(pop)

	static CLIENT_GAME_STATE * clientStates;
	InputEventStruct clientInput[4];
	//static CLIENT_GAME_STATE * aiStates;
	UINT16 numPlayers = 0;
	RakPeerInterface * peer;
	Packet * packet;
	char * names[MAX_PLAYERS];
	UINT8  nameSizes[MAX_PLAYERS];
	UINT8  openIDs[MAX_PLAYERS];
public:
	Server();
	~Server();

	int init(uint16_t port);
	int update();
	void stop();
	bool Shutdown();
	XMFLOAT3 getLocation(unsigned int index) { return clientStates[index].position; }
	XMFLOAT3 getRotation(unsigned int index) { return clientStates[index].rotation; }
	void setStates(unsigned int index, bool hasBall, XMFLOAT3 pos, XMFLOAT3 rot);
	void sendPackets();
	void setObjectCount(int count);
	int getNewState() { return lastState; }
	InputEventStruct * getInputEvent(unsigned int index) { return &clientInput[index]; }
	CLIENT_GAME_STATE * getState(unsigned int i) { return &clientStates[i]; }

private:
	int lastState = 0;
	int packRec = 0;
	int serverObjs = 0;
	bool shutdown = false;
	int shutdowntimer = 0;
	void sendMessage(char * message, GameMessages ID, bool broadcast);
	void sendMessage(char * message, unsigned int length, GameMessages ID, bool broadcast);
	void rerouteMessage();
	UINT16 registerClient();
	void sendNew();
	void unregisterClient();
	void sendDisconnect();
	void recievePacket();
	void recieveInput();
};

