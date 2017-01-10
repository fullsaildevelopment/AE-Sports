#pragma once

#define _WINSOCKAPI_


#ifdef CLIENTDLL_EXPORTS
#define CLIENTDLL_API __declspec(dllexport)
#else
#define CLIENTDLL_API __declspec(dllimport)
#endif

#include "..\TWS Network\RakNet\WindowsIncludes.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "..\TWS Network\RakNet\Gets.h"

#include "..\TWS Network\RakNet\RakPeerInterface.h"
#include "..\TWS Network\RakNet\MessageIdentifiers.h"
#include "..\TWS Network\RakNet\BitStream.h"
#include "..\TWS Network\RakNet\RakNetTypes.h"
#include "..\TWS Network\RakNet\Router2.h"



using namespace RakNet;
using namespace std;

class CLIENTDLL_API Client
{
private:

	enum GameMessages
	{
		ID_TEST = ID_USER_PACKET_ENUM + 1,
		ID_CLIENT_MESSAGE,
		ID_SERVER_MESSAGE,
		ID_CLIENT_DISCONNECT,
		ID_SERVER_CLOSURE,
		ID_CLIENT_REGISTER,
		ID_REQUEST
	};
	
	static RakPeerInterface * peer;
	static Packet * packet;
	static char * address;
public:

	Client();
	~Client();

	static int init(char* address, UINT16 port);
	int run();
	void stop();

	int sendInput(UINT8 keyUp, UINT8 keyDown, UINT8 keyQuit);


private:
	UINT16 clientID;
	char clientName[12];
	int sendAlive();
	void sendClose();
	void sendMessage(char * message, GameMessages ID);
	void readMessage();
	void GetID();
	void registerName();
};

