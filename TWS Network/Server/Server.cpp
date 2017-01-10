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
			string newMessage = "Please welcome the new player, ";
			newMessage += names[id];
			sendMessage((char*)newMessage.c_str(), ID_SERVER_MESSAGE);
			break;
		}
		case ID_REQUEST:
		{
			sendNew();
			break;
		}

		default:
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
	}

	return 1;
}

void Server::stop()
{
	peer->DeallocatePacket(packet);
	RakPeerInterface::DestroyInstance(peer);
}


void Server::sendMessage(char * message, GameMessages ID)
{
	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID);
	bsOut.Write(message);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}

void Server::rerouteMessage() // sends incoming message to everyone else
{
	BitStream bsOut(packet->data, packet->length, false);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}

UINT16 Server::registerClient()
{
	RakString rs;
	BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));

	UINT16 clientID;
	char cID;
	string temp;
	bsIn.Read(clientID);
	bsIn.Read(temp);
	
	//clientID = atoi(&cID);
	//bsIn.IgnoreBytes(sizeof(UINT16));
	//bsIn.Read(temp);
	//names[clientID] = temp.c_str();
	return clientID;
}

void Server::sendNew()
{
	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_CLIENT_REGISTER);
	bsOut.Write(numPlayers);
	++numPlayers;

	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

}