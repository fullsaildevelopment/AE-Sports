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
	struct CLIENT_GAME_STATE
	{
		UINT8 clientID;
		UINT8 nameLength;
		char animationName[125];
		bool hasBall = false;
		XMFLOAT4X4 world;

		CLIENT_GAME_STATE() {}
	};
#pragma pack(pop)

	static RakPeerInterface * peer;
	static Packet * packet;
	static char * address;
	static CLIENT_GAME_STATE * myState;
	static CLIENT_GAME_STATE * clientStates;
public:

#pragma pack(push, 1)
	struct KeyStates
	{
		UINT8 up;
		UINT8 down;
		UINT8 left;
		UINT8 right;
	};
#pragma pack(pop)
	Client();
	~Client();

	int init(char* address, UINT16 port);
	int run();
	void stop();

	int sendInput(KeyStates * states);
	void sendStop();
	void sendMessage(char * newMessage);
	void sendPacket();
	UINT8 getID();
	CLIENT_GAME_STATE getState(unsigned int index);

	XMFLOAT4X4 getLocation(unsigned int index) { return clientStates[index].world; }
	int getNumPackets() { return numPackets; }
	void setLocation(XMFLOAT4X4 loc) { myState->world = loc; }

private:
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
	void recievePackets();
};

