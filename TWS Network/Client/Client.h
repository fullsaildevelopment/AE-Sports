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
#include "..\TWS Network\RakNet\Kbhit.h"



using namespace RakNet;
using namespace std;

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
		ID_REMOVE_CLIENT
	};
	
	static RakPeerInterface * peer;
	static Packet * packet;
	static char * address;
public:

	Client();
	~Client();

	int init(char* address, UINT16 port);
	int run();
	void stop();

	int sendInput(UINT8 keyUp, UINT8 keyDown, UINT8 keyQuit);
	void sendStop();
	void sendMessage(char * newMessage);

private:

	bool disconnect = false;
	UINT8 clientID;
	char clientName[8];
	void sendMessage(char * message, GameMessages ID);
	void sendMessage(char * message, GameMessages ID, SystemAddress sAddress);
	void sendMessage(UINT8 clientid, GameMessages ID, SystemAddress sAddress);
	void readMessage();
	void GetID();
	void registerName();
};

