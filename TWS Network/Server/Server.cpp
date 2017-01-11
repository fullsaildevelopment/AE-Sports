#include "Server.h"




Server::Server()
{

}

Server::~Server()
{
	stop();
}

int Server::init(uint16_t port)
{
	peer = RakNet::RakPeerInterface::GetInstance();

	SocketDescriptor sd(port, 0);
	peer->Startup(MAX_PLAYERS, &sd, 1);

	printf("Starting the server.\n");
	peer->SetMaximumIncomingConnections(MAX_PLAYERS);

	for (unsigned int i = 0; i < MAX_PLAYERS; ++i)
	{

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
			break;
			}
		case ID_CONNECTION_LOST:
			{
				printf("A client lost the connection.\n");
			break;
			}

		case ID_TEST:
		{
			rerouteMessage();
		break;
		}
		case ID_CLIENT_REGISTER:
		{
			UINT16 id = registerClient();
			char newMessage[50] = "Please welcome the new player, ";
			memcpy(&newMessage[strlen(newMessage)], names[id], nameSizes[id]);
			memcpy(&newMessage[strlen(newMessage)], ".", 2);
			sendMessage(newMessage, ID_SERVER_MESSAGE, true);

			sendMessage(newMessage, ID_SERVER_MESSAGE, false);
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
			
			break;
		}
		}
	}

	return 1;
}

void Server::stop()
{
	peer->DeallocatePacket(packet);
	RakPeerInterface::DestroyInstance(peer);
}


void Server::sendMessage(char * message, GameMessages ID, bool broadcast)
{
	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID);
	bsOut.Write(message);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, broadcast);
}

void Server::rerouteMessage() // sends incoming message to everyone else
{
	BitStream bsOut(packet->data, packet->length, false);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);
}

UINT16 Server::registerClient()
{
	RakString rs;
	BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));

	UINT8 length, clientid;
	char temp[8];
	bsIn.Read(clientid);
	bsIn.Read(length);
	bsIn.Read(temp, length);

	names[clientid - 1] = temp;
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

	openIDs[leavingID] = leavingID + 1;
	char message[100];
	memcpy(&message[0], names[leavingID], nameSizes[leavingID]);
	memcpy(&message[nameSizes[leavingID]], " has left the lobby.\n", strlen(" has left the lobby.\n"));
	sendMessage(message, ID_CLIENT_DISCONNECT, true);
}