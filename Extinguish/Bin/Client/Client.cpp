#include "Client.h"

RakPeerInterface * Client::peer = nullptr;

Packet * Client::packet = nullptr;

char * Client::address = nullptr;

//Client::CLIENT_GAME_STATE * Client::myState = new CLIENT_GAME_STATE();
//Client::CLIENT_GAME_STATE * Client::clientStates = new CLIENT_GAME_STATE[30];
//Client::GAME_STATE * Client::gameState = new GAME_STATE();


Client::Client()
{
	myState = new std::vector<CLIENT_GAME_STATE>();
	myState[0].resize(1);
	clientStates = new std::vector<CLIENT_GAME_STATE>();
	gameState = new std::vector<GAME_STATE>();
	gameState[0].resize(MAX_PLAYERS);
	floorState = new std::vector<XMFLOAT3>();
	floorState[0].resize(2090);

	for (int i = 0; i < gameState->size(); ++i)
	{
		gameState[0][i].name = nullptr;
	}
}

Client::~Client()
{
	delete myState;
	delete clientStates;
	delete gameState;


	for (int i = 0; i < gameState->size(); ++i)
	{
		delete gameState[0][i].name;
	}

	if (peer)
	peer->DestroyInstance(peer);
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
			//printf("Connection lost.\n");
			peer->Shutdown(100);
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
			//printf("Please enter a name (8 characters max): ");
			//memcpy(clientName, "Player", strlen("Player"));
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
			peer->Shutdown(100);
			return 0;
			break;
		}
		case ID_INCOMING_PACKET:
		{
			receivePackets();
			if (result != 4 || result != 5)
				result = 2;
			break;
		}
		case ID_SERVER_CLOSURE:
		{
			sendStop();
			break;
		}
		case ID_INCOMING_STATE:
		{
			receiveGameState();
			result = 4;
			break;
		}
		case ID_NEW_CLIENT:
		{
			BitStream bIn(packet->data, packet->length, false);
			bIn.IgnoreBytes(sizeof(MessageID));
			bIn.Read(curNumOfClients);
			result = 5;
			break;
		}
		case ID_START_GAME:
		{
			BitStream bIn(packet->data, packet->length, false);
			bIn.IgnoreBytes(sizeof(MessageID));
			UINT8 temp;
			bIn.Read(temp);
			if (temp == 11)
			{
				gameState[0][0].paused = false;
				gameStart = true;
				return 6;
			}

			break;
		}
		case ID_CLIENT_OBJ:
		{
			GetID();
			if (!gameStart)
				return 7;
			break;
		}
		case ID_INCOMING_FLOOR:
		{
			receiveFloor();
			break;
		}
		case ID_INCOMING_MESSAGE:
		{	ReceiveMessage();
		result = 69;
		break; }
		}
	}

	return result;
}

void Client::stop()
{
	if (peer) {
		/*while (true)
		{*/
			peer->Shutdown(100);
			//int result = run();
			//if (result == 0)
			//	break;
		//	sendStop();
		//}
	}
	//peer->DeallocatePacket(packet);
	//RakPeerInterface::DestroyInstance(peer);
}

int Client::sendInput(bool keyboard[256], bool keyboardDown[256], bool keyboardUp[256], bool mouse[3], bool mouseDown[3], bool mouseUp[3], int mouseX, int mouseY, bool isServer)
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
	bsOut.Write((UINT16)mouseX);
	bsOut.Write((UINT16)mouseY);
	bsOut.Write(isServer);

	peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetSystemAddressFromIndex(0), false);
	return 1;
}

void Client::sendMessage(char* message, uint16_t stride)
{
	BitStream bsOut;

	bsOut.Write((RakNet::MessageID)ID_INCOMING_MESSAGE);
	bsOut.Write(stride);

	for (uint16_t i = 0; i < stride; ++i)
	{
		bsOut.Write(message[i]);
	}

	peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetSystemAddressFromIndex(0), false);
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

void Client::ReceiveMessage()
{
	BitStream bIn(packet->data, packet->length, false);
	bIn.IgnoreBytes(sizeof(MessageID));

	bIn.Read(messageID);
	bIn.Read(stride);

	delete message;
	message = nullptr;

	message = new char[stride];

	for (int i = 0; i < stride; ++i)
	{
		bIn.Read(message[i]);
	}
}

void Client::GetID()
{
	if (!gameStart) {
		BitStream bsIn(packet->data, packet->length, false);
		bsIn.IgnoreBytes(sizeof(MessageID));
		bsIn.Read(clientID);
		bsIn.Read(objects);
	}
//	clientStates = new CLIENT_GAME_STATE[objects]();
}

void Client::registerName()
{
	char test[10];
	test[0] = clientID;
	test[1] = (char)strlen("Player");
	strcpy(&test[2], "Player");
	
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
	myState[0][0].clientID = clientID;
	//clientState->timeStamp = RakNet::GetTime();
	bOut.Write((MessageID)ID_INCOMING_PACKET);
	//bOut.Write(clientState->timeStamp);
	bOut.Write(clientID);
//	bOut.Write(myState->nameLength);
//	bOut.Write(myState->animationName, myState->nameLength);
	bOut.Write(myState[0][0].hasBall);

	//bOut.Write(myState->world);
	bOut.Write(myState[0][0].position);
	bOut.Write(myState[0][0].rotation);
	bOut.Write(myState[0][0].parentIndex);
	bOut.Write(myState[0][0].animationIndex);

	peer->Send(&bOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetSystemAddressFromIndex(0), false);
}

void Client::receivePackets()
{
	if (clientStates[0].size() >= 0)
	{
		BitStream bIn(packet->data, packet->length, false);
		bIn.IgnoreBytes(sizeof(MessageID));
		//bIn.Read(clientState->timeStamp);


		bIn.Read(objects);

		numPackets = (int)objects;
		clientStates[0].clear();
		clientStates[0].resize(numPackets);
		// for each game object, read in the data of the object state

		for (unsigned int i = 0; i < (unsigned int)objects; ++i)
		{
			bIn.Read(clientStates[0][i].clientID);
			//		bIn.Read(clientStates[i].nameLength);
			//		bIn.Read(clientStates[i].animationName, (unsigned int)clientStates[i].nameLength);
			bIn.Read(clientStates[0][i].hasBall);
			//	bIn.Read(clientStates[i].world);
			bIn.Read(clientStates[0][i].position);
			bIn.Read(clientStates[0][i].rotation);
			bIn.Read(clientStates[0][i].parentIndex);
			bIn.Read(clientStates[0][i].animationIndex);
			bIn.Read(clientStates[0][i].otherIndex);
			bIn.Read(clientStates[0][i].transitionIndex);
			bIn.Read(clientStates[0][i].soundID);
			bIn.Read(clientStates[0][i].hasSound);
		}
	}
}

Client::CLIENT_GAME_STATE Client::getMyState()
{
	// return the state of the obj at index
	return myState[0][0];
}

void Client::receiveGameState()
{
	BitStream bIn(packet->data, packet->length, false);
	bIn.IgnoreBytes(sizeof(MessageID));

	bIn.Read(gameState[0][0].scene);
	bIn.Read(gameState[0][0].paused);
	bIn.Read(gameState[0][0].countdown);
	bIn.Read(gameState[0][0].scoreA);
	bIn.Read(gameState[0][0].scoreB);
	bIn.Read(gameState[0][0].time);
	bIn.Read(gameState[0][0]._dt);
	for (unsigned int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (i == clientID - 1)
		{
			bIn.Read(gameState[0][0].sprintA);
			bIn.Read(gameState[0][0].sprintD);
			bIn.Read(gameState[0][0].down);
		}
		else 
		{
			bool temp, temp2;
			bIn.Read(temp);
			bIn.Read(temp2);
			bIn.Read(temp2);
		}

		bIn.Read(gameState[0][i].score);
		bIn.Read(gameState[0][i].assists);
		bIn.Read(gameState[0][i].saves);
		bIn.Read(gameState[0][i].goals);

		UINT8 nameLength;
		bIn.Read(nameLength);

		delete gameState[0][i].name;
		gameState[0][i].name = new char[nameLength + 1];
		bIn.Read(gameState[0][i].name, nameLength);
		gameState[0][i].name[nameLength] = '\0';
	}
}


void Client::changeTeam(UINT16 team)
{
	if (team == 0)
	{
		sendMessage(clientID, ID_CHANGE_TEAM_A, peer->GetSystemAddressFromIndex(0));
	}
	else
	{
		sendMessage(clientID, ID_CHANGE_TEAM_B, peer->GetSystemAddressFromIndex(0));
	}
}

void Client::sendEmpty(bool empty)
{
	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_SPRINT_EMPTY);
	bsOut.Write(clientID);
	bsOut.Write(empty);
	peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, peer->GetSystemAddressFromIndex(0), false);
}

void Client::receiveFloor()
{
	BitStream bIn(packet->data, packet->length, false);
	bIn.IgnoreBytes(sizeof(MessageID));

	UINT8 numFloor;
	bIn.Read(numFloor);
	//floorState[0].clear();

	for (unsigned int i = 0; i < (unsigned int)numFloor; ++i)
	{
		UINT8 index;
		bIn.Read(index);
		XMFLOAT3 hex;
		bIn.Read(hex.x);
		bIn.Read(hex.y);
		bIn.Read(hex.z);
		floorState[0][index] = hex;
	}
}