#pragma once

#define _WINSOCKAPI_


#ifdef CLIENTDLL_EXPORTS
#define CLIENTDLL_API __declspec(dllexport)
#else
#define CLIENTDLL_API __declspec(dllimport)
#endif

#include "..\RakNet\WindowsIncludes.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>

#include "..\RakNet\Gets.h"
#include "..\RakNet\RakPeerInterface.h"
#include "..\RakNet\MessageIdentifiers.h"
#include "..\RakNet\BitStream.h"
#include "..\RakNet\RakNetTypes.h"
#include "..\RakNet\Router2.h"
#include "..\RakNet\Kbhit.h"
#include "..\RakNet\GetTime.h"


#include <DirectXMath.h>


using namespace RakNet;
using namespace std;
using namespace DirectX;

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
		ID_START_GAME
	};

	static RakPeerInterface * peer;
	static Packet * packet;
	static char * address;
//	static CLIENT_GAME_STATE * myState;
//	static CLIENT_GAME_STATE * clientStates;
//	static GAME_STATE * gameState;
	std::vector<CLIENT_GAME_STATE> clientStates;
	std::vector<CLIENT_GAME_STATE> myState;
	std::vector<GAME_STATE> gameState;

	UINT8 curNumOfClients;
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

	int sendInput(bool keyboard[256], bool keyboardDown[256], bool keyboardUp[256], bool mouse[3], bool mouseDown[3], bool mouseUp[3], int mouseX, int mouseY, int clientID, bool isServer);
	void sendStop();
	void sendMessage(char * newMessage);
	void sendPacket();
	UINT8 getID() { return clientID; }
	CLIENT_GAME_STATE getState(unsigned int index);
	//UINT8 hasBall(unsigned int index) { return clientStates[index].hasBall; }
	bool hasBall(unsigned int index) { return clientStates[index].hasBall; }
	INT8 GetParentIndex(unsigned int index) { return clientStates[index].parentIndex; }
	INT8 GetAnimationIndex(unsigned int index) { return clientStates[index].animationIndex; }
	INT8 GetTransitionIndex(unsigned int index) { return clientStates[index].transitionIndex; }
	UINT32 GetSoundID(unsigned int index) { return clientStates[index].soundID; }
	bool HasSound(unsigned int index) { return clientStates[index].hasSound; }
	XMFLOAT3 getLocation(unsigned int index) { return clientStates[index].position; }
	XMFLOAT3 getRotation(unsigned int index) { return clientStates[index].rotation; }
	int getNumPackets() { return numPackets; }
	void setLocation(XMFLOAT3 loc) { myState[0].position = loc; }
	void setRotation(XMFLOAT3 rot) { myState[0].rotation = rot; }
	int getFloorState(unsigned int i) { return clientStates[i].otherIndex; }
	UINT8 getScoreA() { return gameState[0].scoreA; }
	UINT8 getScoreB() { return gameState[0].scoreB; }
	float getTime() { return gameState[0].time; }
	UINT8 getNumClients() { return curNumOfClients; }

private:
	UINT8 objects;
	int numPackets = 0;
	bool disconnect = false;
	UINT8 clientID;
	char clientName[8];
	void sendMessage(char * message, GameMessages ID);
	void sendMessage(char * message, GameMessages ID, SystemAddress sAddress);
	void sendMessage(UINT8 clientid, GameMessages ID, SystemAddress sAddress);
	void readMessage();
	void GetID();
	void registerName();
	void receivePackets();
	void receiveGameState();
};

