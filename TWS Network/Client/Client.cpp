#include "Client.h"

RakPeerInterface * Client::peer = nullptr;

Packet * Client::packet = nullptr;

char * Client::address = nullptr;

Client::Client()
{
}

Client::~Client()
{
}

int Client::init(char* _address, UINT16 port)
{
	SocketDescriptor sd(port, 0);
	peer = RakNet::RakPeerInterface::GetInstance();
	sd.socketFamily = AF_INET;

	peer->Startup(1, &sd, 1);
	address = _address;
	
	peer->Ping("255.255.255.255", port, true);

	//peer->Connect(address, port, 0, 0);
	
	return 1;
}

int Client::run()
{
	for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
	{
		switch (packet->data[0])
		{
		case ID_UNCONNECTED_PONG:
		{
			ConnectionAttemptResult result = peer->Connect(packet->systemAddress.ToString(false), 60000, 0, 0);
			if (result != CONNECTION_ATTEMPT_STARTED)
			{
				float temp = 0;
			}
			else
			{
				printf("Got pong from %s\n", packet->systemAddress.ToString());
			}

			break;
		}
		case ID_CONNECTION_ATTEMPT_FAILED:
		{
			return 0;
			break;
		}
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			printf("Connected to server.\nWelcome to the lobby.\n");

			sendMessage("idpls", ID_REQUEST);
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
			break;
		}
		case ID_NEW_INCOMING_CONNECTION:
		{
			printf("A connection is incoming.\n");
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
		case ID_CLIENT_MESSAGE:
		{
			readMessage();
			break;
		}
		case ID_CLIENT_REGISTER:
		{
			GetID();
			printf("Please enter a name (8 characters max): ");
			cin >> clientName;
			registerName();
			break;
		}
		case ID_SERVER_MESSAGE:
		{
			readMessage();
			break;
		}
		case ID_REMOVE_CLIENT:
		{
			stop();
			return 0;
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

void Client::sendMessage(char * message, GameMessages ID)
{
	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID);
	bsOut.Write(message, (unsigned int)strlen(message));
//	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
	peer->SetOfflinePingResponse(message, (unsigned int)strlen(message));
}

void Client::sendMessage(char * message, GameMessages ID, SystemAddress sAddress)
{

	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID);
	bsOut.Write(message, (unsigned int)strlen(message));
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, sAddress, false);
}


void Client::sendMessage(UINT8 clientid, GameMessages ID, SystemAddress sAddress)
{

	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID);
	bsOut.Write(clientid);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, sAddress, false);
}

void Client::readMessage()
{
	RakString rs;
	BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));
	bsIn.Read(rs);
	int endPoint = rs.Find("\n");
	if (endPoint > 0 && endPoint < rs.GetLength())
	{
//		rs.Erase(endPoint, rs.GetCharacterCount);
		printf("%s\n", rs.SubStr(0, endPoint).C_String());
	}
	else
		printf("%s\n", rs.C_String());
}

void Client::GetID()
{
	RakString rs;
	BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));
	bsIn.Read(clientID);
}

void Client::registerName()
{
	char test[10];
	test[0] = clientID;
	test[1] = (char)strlen(clientName);
	strcpy(&test[2], clientName);
	
	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_CLIENT_REGISTER);
	bsOut.Write(test, (unsigned int)strlen(test));

	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}

void Client::sendStop()
{
	disconnect = true;
	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_CLIENT_DISCONNECT);
	bsOut.Write(clientID);
	peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetSystemAddressFromIndex(0), false);

	peer->CloseConnection(peer->GetGUIDFromIndex(0), true, '\000', IMMEDIATE_PRIORITY);
}

void Client::sendMessage(char * newMessage)
{
	sendMessage(newMessage, ID_CLIENT_MESSAGE, peer->GetSystemAddressFromIndex(0));
}