#include "Client.h"

RakPeerInterface * Client::peer = nullptr;

Packet * Client::packet = nullptr;

char * Client::address = nullptr;

Client::CLIENT_GAME_STATE * Client::myState = new CLIENT_GAME_STATE();
Client::CLIENT_GAME_STATE * Client::clientStates = new CLIENT_GAME_STATE[19];
//std::vector<Client::CLIENT_GAME_STATE> * states = new std::vector<Client::CLIENT_GAME_STATE>();


Client::Client()
{
	//XMStoreFloat4x4(&myState->world, XMMatrixIdentity());
	memcpy(myState->animationName, "name here", strlen("name here"));
	myState->nameLength = (UINT8)strlen("name here");
	//states->resize(16);
}

Client::~Client()
{
	delete myState;
	/*for (unsigned int i = 0; i < objects; ++i)
	{
		delete &clientStates[i];
	}*/
	//delete[] clientStates;
}

int Client::init(char* _address, UINT16 port)
{
	SocketDescriptor sd(port, 0);
	peer = RakNet::RakPeerInterface::GetInstance();
	sd.socketFamily = AF_INET;

	StartupResult res = peer->Startup(1, &sd, 1);

		UINT16 newPort = port;
	if (res == SOCKET_PORT_ALREADY_IN_USE)
	{

		while (res == SOCKET_PORT_ALREADY_IN_USE)
		{
			newPort += 1;
			sd = SocketDescriptor(newPort, 0);
			res = peer->Startup(1, &sd, 1);
		}
	}
	address = _address;
	
	peer->Ping("255.255.255.255", 60000, true);

	//printf("Pinging\n");
	//ConnectionAttemptResult temp = peer->Connect(address, 60000, 0, 0);
	
	return 1;
}

int Client::run()
{
	int result = 1;
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
			result = 3;
			printf("Please enter a name (8 characters max): ");
			memcpy(clientName, "Player", strlen("Player"));
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
		case ID_INCOMING_PACKET:
		{
			recievePackets();
			result = 2;
			break;
		}
		case ID_SERVER_CLOSURE:
		{
			sendStop();
			break;
		}
		}
	}

	return result;
}

void Client::stop()
{
	peer->DeallocatePacket(packet);
	RakPeerInterface::DestroyInstance(peer);
}

int Client::sendInput(bool keyboard[256], bool keyboardDown[256], bool keyboardUp[256], bool mouse[3], bool mouseDown[3], bool mouseUp[3], int mouseX, int mouseY, int clientID, bool isServer)
{
	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_INCOMING_INPUT);
	//bsOut.Write((UINT8)sizeof(InputEventStruct));
	//bsOut.Write(states);
	bsOut.Write((UINT8)clientID);
	for (unsigned int i = 0; i < 256; ++i)
		bsOut.Write(keyboard[i]);
	for (unsigned int i = 0; i < 256; ++i)
		bsOut.Write(keyboardDown[i]);
	for (unsigned int i = 0; i < 256; ++i)
		bsOut.Write(keyboardUp[i]);

	for (unsigned int i = 0; i < 3; ++i)
		bsOut.Write(mouse[i]);
	for (unsigned int i = 0; i < 3; ++i)
		bsOut.Write(mouseDown[i]);
	for (unsigned int i = 0; i < 3; ++i)
		bsOut.Write(mouseUp[i]);
	bsOut.Write((UINT8)mouseX);
	bsOut.Write((UINT8)mouseY);
	bsOut.Write(isServer);

	peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetSystemAddressFromIndex(0), false);
	return 1;
}

void Client::sendMessage(char * message, GameMessages ID)
{
	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID);
	bsOut.Write(message, (unsigned int)strlen(message));
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
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
	int endPoint = (int)rs.Find("\n");
	if (endPoint > 0 && endPoint < rs.GetLength())
	{
		printf("%s\n", rs.SubStr(0, endPoint).C_String());
	}
	else
		printf("%s\n", rs.C_String());
}

void Client::GetID()
{
	//UINT8 objs;
	RakString rs;
	BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));
	bsIn.Read(clientID);
	bsIn.Read(objects);

//	clientStates = new CLIENT_GAME_STATE[objects]();
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

void Client::sendPacket()
{
	BitStream bOut;
	myState->clientID = clientID;
	//clientState->timeStamp = RakNet::GetTime();
	bOut.Write((MessageID)ID_INCOMING_PACKET);
	//bOut.Write(clientState->timeStamp);
	bOut.Write(clientID);
//	bOut.Write(myState->nameLength);
//	bOut.Write(myState->animationName, myState->nameLength);
	bOut.Write(myState->hasBall);

	//bOut.Write(myState->world);
	bOut.Write(myState->position);
	bOut.Write(myState->rotation);
	bOut.Write(myState->parentIndex);

	peer->Send(&bOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetSystemAddressFromIndex(0), false);
}

void Client::recievePackets()
{
	if (clientStates)
	{
		BitStream bIn(packet->data, packet->length, false);
		bIn.IgnoreBytes(sizeof(MessageID));
		//bIn.Read(clientState->timeStamp);


		bIn.Read(objects);

		numPackets = (int)objects;
		// for each game object, read in the data of the object state

		for (unsigned int i = 0; i < (unsigned int)objects; ++i)
		{
			bIn.Read(clientStates[i].clientID);
			//		bIn.Read(clientStates[i].nameLength);
			//		bIn.Read(clientStates[i].animationName, (unsigned int)clientStates[i].nameLength);
			bIn.Read(clientStates[i].hasBall);
			//	bIn.Read(clientStates[i].world);
			bIn.Read(clientStates[i].position);
			bIn.Read(clientStates[i].rotation);
			bIn.Read(clientStates[i].parentIndex);
		}
	}
}

Client::CLIENT_GAME_STATE Client::getState(unsigned int index)
{
	// return the state of the obj at index
	return *myState;
}