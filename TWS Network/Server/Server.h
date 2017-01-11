#pragma once

#ifdef SERVERDLL_EXPORTS
#define SERVERDLL_API __declspec(dllexport)
#else
#define SERVERDLL_API __declspec(dllimport)
#endif

#include <stdio.h>
#include <string.h>

#include "..\TWS Network\RakNet\RakPeerInterface.h"
#include "..\TWS Network\RakNet\MessageIdentifiers.h"
#include "..\TWS Network\RakNet\BitStream.h"
#include "..\TWS Network\RakNet\RakNetTypes.h"
#include "..\TWS Network\RakNet\Router2.h"

using namespace RakNet;
using namespace std;

#define MAX_PLAYERS 4

class SERVERDLL_API Server
{
private:

	enum GameMessages
	{
		ID_TEST = ID_USER_PACKET_ENUM + 1
	};

	int numPlayers = 0;
	RakPeerInterface * peer;
	Packet * packet;
public:
	Server();
	~Server();

	int init(uint16_t port);
	int update();
	void stop();

private:

	void sendMessage(char * message);
	void readMessage();
};

