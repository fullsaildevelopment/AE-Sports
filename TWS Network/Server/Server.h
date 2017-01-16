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
		ID_CLIENT_MESSAGE = ID_USER_PACKET_ENUM + 2,
		ID_SERVER_MESSAGE,
		ID_CLIENT_DISCONNECT,
		ID_SERVER_CLOSURE,
		ID_CLIENT_REGISTER,
		ID_REQUEST,
		ID_INCOMING_PACKET,
		ID_REMOVE_CLIENT
	};

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

private:

	void sendMessage(char * message, GameMessages ID, bool broadcast);
	void sendMessage(char * message, unsigned int length, GameMessages ID, bool broadcast);
	void rerouteMessage();
	UINT16 registerClient();
	void sendNew();
	void unregisterClient();
	void sendDisconnect();
};

