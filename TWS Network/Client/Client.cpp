#include "Client.h"

RakPeerInterface * Client::peer = nullptr;

Packet * Client::packet = nullptr;

Client::Client()
{
}

Client::~Client()
{
	stop();
}

int Client::init(char* address, UINT16 port)
{
	SocketDescriptor sd;
	peer = RakNet::RakPeerInterface::GetInstance();

	peer->Startup(1, &sd, 1);

	peer->Connect("127.0.0.1", 60000, 0, 0);

	/*
	packet = peer->Receive(); 

	if (packet->data[0] != ID_CONNECTION_REQUEST_ACCEPTED)
	{
		peer->DeallocatePacket(packet);
		return 0;
	}*/

	


	return 1;
}

int Client::run()
{
	//Packet * packet;
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
	{
		switch (packet->data[0])
		{
		case ID_CONNECTION_ATTEMPT_FAILED:
		{
			return 0;
			break;
		}
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			printf("Connected to server.\nWelcome to the lobby.\n");
		break;
		}
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
		{
			printf("Another client has disconnected.\n");
			break;
		}
		case ID_REMOTE_CONNECTION_LOST:
		{	
			printf("Another client has lost the connection.\n");
		break;
		}
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
		{
			printf("Another client has connected.\n");
			sendMessage("Hello, newb!\n");
			break;
		}
		case ID_NEW_INCOMING_CONNECTION:
		{
			printf("A connection is incoming.\n");
			sendMessage("Hello, newb!\n");
		break; 
		}
		case ID_NO_FREE_INCOMING_CONNECTIONS:
		{
			printf("The server is full.\n");
			return 0;
		break; 
		}
		case ID_DISCONNECTION_NOTIFICATION:
		{
			printf("We have been disconnected.\n");
			return 0;
		break;
		}
		case ID_CONNECTION_LOST:
		{
			printf("Connection lost.\n");
			return 0;
		break;
		}
		case ID_TEST:
		{
			readMessage();
			break;
		}
		default:
		{
			printf("Message with identifier %i has arrived.\n", packet->data[0]);
			break;
		}
		}
	}

	return 1;
}

void Client::stop()
{
	peer->DeallocatePacket(packet);
	RakPeerInterface::DestroyInstance(peer);
}

int Client::sendInput(UINT8 keyUp, UINT8 keyDown, UINT8 keyQuit) // inputs need to change
{
	return 0;
}


void Client::sendClose()
{

}

int Client::sendAlive()
{
	return 0;
}

void Client::sendMessage(char * message)
{
	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_TEST);
	bsOut.Write(message);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}

void Client::readMessage()
{
	RakString rs;
	BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));
	bsIn.Read(rs);
	printf("%s\n", rs.C_String());
}