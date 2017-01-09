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
		ID_TEST = ID_USER_PACKET_ENUM + 1
	};
	
public:

	static RakPeerInterface * peer;
	static Packet * packet;
	Client();
	~Client();

	static int init(char* address, UINT16 port);
	int run();
	void stop();

	int sendInput(UINT8 keyUp, UINT8 keyDown, UINT8 keyQuit);


private:
	int sendAlive();
	void sendClose();
	void sendMessage(char * message);
	void readMessage();
};

