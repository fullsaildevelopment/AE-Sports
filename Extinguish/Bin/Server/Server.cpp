#include "Server.h"

Server::CLIENT_GAME_STATE * Server::clientStates = new CLIENT_GAME_STATE[MAX_PLAYERS]();

Server::Server()
{

}

Server::~Server()
{
	for (unsigned int i = 0; i < MAX_PLAYERS; ++i)
		delete names[i];

	//delete clientState;
}

int Server::init(uint16_t port)
{
	peer = RakNet::RakPeerInterface::GetInstance();

	SocketDescriptor sd(port, 0);
	sd.socketFamily = AF_INET;
	StartupResult res = peer->Startup(MAX_PLAYERS, &sd, 1);

	if (res == SOCKET_FAILED_TO_BIND)
		float temp = 0;

	printf("Starting the server.\n");
	peer->SetMaximumIncomingConnections(MAX_PLAYERS);

	printf("Server made at: %s\n", peer->GetMyBoundAddress().ToString());
	for (unsigned int i = 0; i < MAX_PLAYERS; ++i)
	{
		names[i] = new char[8];
		openIDs[i] = i + 1;
	}
	return 1;
}

int  Server::update()
{
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			printf("Another client has disconnected.\n");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			printf("Another client has lost the connection.\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			printf("Another client has connected.\n");
			break;
		case ID_NEW_INCOMING_CONNECTION:
			printf("A connection is incoming.\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
		{
			printf("The server is full.\n");
			break;
		}
		case ID_DISCONNECTION_NOTIFICATION:
		{
			printf("A client has disconnected.\n");

			if (numPlayers == 0)
				return 0;
			break;
		}
		case ID_CONNECTION_LOST:
		{
			printf("A client lost the connection.\n");
			break;
		}
		case ID_CLIENT_MESSAGE:
		{
			printf("Incoming client to client message\n");
			rerouteMessage();
			break;
		}
		case ID_CLIENT_REGISTER:
		{
			UINT16 id = registerClient();
			char newMessage[50] = "Please welcome the new player, ";
			memcpy(&newMessage[strlen(newMessage)], names[id], nameSizes[id]);
			memcpy(&newMessage[strlen(newMessage)], ".\n", 3);

			printf(newMessage);
			sendMessage(newMessage, ID_SERVER_MESSAGE, true);
			break;
		}
		case ID_REQUEST:
		{
			sendNew();
			break;
		}
		case ID_CLIENT_DISCONNECT:
		{
			unregisterClient();
			sendDisconnect();

			break;
		}
		case ID_INCOMING_PACKET:
		{
			recievePacket();
			sendPackets(); // for testing purposes
			break;
		}
		}
	}


	return 1;
}

void Server::stop()
{
	shutdown = true;
	RakPeerInterface::DestroyInstance(peer);
}

bool Server::Shutdown()
{
	printf("\nClosing server\n");

	sendMessage("", ID_SERVER_CLOSURE, true);
	if (numPlayers == 0)
	{
		shutdown = true;
		return true;
	}

	return false;
}

void Server::sendMessage(char * message, GameMessages ID, bool broadcast)
{
	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID);
	bsOut.Write(message);
	if (!broadcast)
		peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, broadcast);
	else if (broadcast && ID != ID_SERVER_CLOSURE)
		peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), broadcast);
	else
		peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), broadcast);
}

void Server::sendMessage(char * message, unsigned int length, GameMessages ID, bool broadcast)
{
	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID);
	bsOut.Write(&message[0], length);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, broadcast);
}

void Server::rerouteMessage() // sends incoming message to everyone else
{
	BitStream bsOut;
	BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));
	char message[255];
	UINT8 length;
	bsIn.Read(length);
	bsIn.Read(message, (unsigned int)length);
	bsOut.Write((MessageID)ID_CLIENT_MESSAGE);
	bsOut.Write(message, (unsigned int)strlen(message));
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}

UINT16 Server::registerClient()
{
	RakString rs;
	BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));

	UINT8 length, clientid;
	bsIn.Read(clientid);
	bsIn.Read(length);
	bsIn.Read(names[clientid - 1], length);

	nameSizes[clientid - 1] = length;
	return clientid - 1;
}

void Server::sendNew()
{
	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_CLIENT_REGISTER);
	++numPlayers;
	UINT8 newID;
	for (unsigned int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (openIDs[i] != 0)
		{
			newID = openIDs[i];
			openIDs[i] = 0;
			break;
		}
	}

	bsOut.Write(newID);

	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}

void Server::unregisterClient()
{
	
	BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));

	UINT8 leavingID;
	bsIn.Read(leavingID);
	int size = nameSizes[leavingID - 1];

	openIDs[leavingID - 1] = leavingID;
	char message[30];
	memcpy(&message[0], names[leavingID - 1], size);
	memcpy(&message[size], " has left the lobby.\n", strlen(" has left the lobby.\n"));
	sendMessage(message, ID_SERVER_MESSAGE, true);

	--numPlayers;
}

void Server::sendDisconnect()
{
	sendMessage("", ID_REMOVE_CLIENT, false);
}

void Server::recievePacket()
{
	BitStream bIn(packet->data, packet->length, false);
	bIn.IgnoreBytes(sizeof(MessageID));

	if (packet->data[1] != sizeof(KeyStates))
	{
		CLIENT_GAME_STATE tempState;

		bIn.Read(tempState.timeStamp);
		bIn.Read(tempState.clientID);
		bIn.Read(tempState.nameLength);
		bIn.Read(tempState.animationName, (unsigned int)tempState.nameLength);
		bIn.Read(tempState.hasBall);
		bIn.Read(tempState.world);

		clientStates[tempState.clientID - 1] = tempState;

		/*
		printf("\n\nPACKET RECIEVED\nClient: %s\nClient ID: %i\n", names[tempState.clientID - 1], clientState->clientID);

		printf("Position:\n%f, %f, %f, %f,\n%f, %f, %f, %f,\n%f, %f, %f, %f,\n%f, %f, %f, %f\n",
			clientState->world._11,
			clientState->world._12,
			clientState->world._13,
			clientState->world._14,
			clientState->world._21,
			clientState->world._22,
			clientState->world._23,
			clientState->world._24,
			clientState->world._31,
			clientState->world._32,
			clientState->world._33,
			clientState->world._34,
			clientState->world._41,
			clientState->world._42,
			clientState->world._43,
			clientState->world._44);

		if (clientState->hasBall)
			printf("hasBall: true\n");
		else
			printf("hasBall: false\n");
			*/
	}
	else
	{
		KeyStates tempState;
		UINT8 id;
		bIn.IgnoreBytes(sizeof(UINT8));
		bIn.Read(id);
		bIn.EndianSwapBytes(3, sizeof(KeyStates));
		bIn.Read(tempState);
		//bIn.Read(tempState.up);
		//bIn.Read(tempState.down);
		//bIn.Read(tempState.left);
		//bIn.Read(tempState.right);

		if (tempState.down == 1)
			printf("down\n");
		if (tempState.up == 1)
			printf("up\n");
		if (tempState.left == 1)
			printf("left\n");
		if (tempState.right == 1)
			printf("right\n");
	}
}

void Server::sendPackets()
{
	// send packet x8 to all clients
	// presently only need a max of 4 until merged with engine


	BitStream bOut;
	bOut.Write((MessageID)ID_INCOMING_PACKET);

	for (unsigned int i = 0; i < MAX_PLAYERS; ++i)
	{
		bOut.Write(GetTime());
		// p1
		bOut.Write(clientStates[i].clientID);
		bOut.Write(clientStates[i].nameLength);
		bOut.Write(clientStates[i].animationName, (unsigned int)clientStates[i].nameLength);
		bOut.Write(clientStates[i].hasBall);
		bOut.Write(clientStates[i].world);
	}
	peer->Send(&bOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);
}