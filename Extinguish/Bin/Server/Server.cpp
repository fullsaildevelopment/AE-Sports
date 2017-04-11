#include "Server.h"

//Server::CLIENT_GAME_STATE * Server::clientStates = new CLIENT_GAME_STATE[8];
//Server::CLIENT_GAME_STATE * Server::clientStates =  new CLIENT_GAME_STATE[30];
//Server::GAME_STATE * Server::gameState = new GAME_STATE();

//std::vector<Server::CLIENT_GAME_STATE> clientStates;
//std::vector<Server::GAME_STATE> gameState;


void Server::setObjectCount(int count) { 
	serverObjs = count;
}

Server::Server()
{
	clientStates = new std::vector<CLIENT_GAME_STATE>();
	gameState = new std::vector<GAME_STATE>();
	gameState[0].resize(MAX_PLAYERS);

	for (int i = 0; i < gameState->size(); ++i)
	{
		gameState[0][i].name = nullptr;
	}

	for (unsigned int i = 0; i < 6; ++i)
	{
		pUp.positions[i] = { 0,0,0 };
		pUp.elapsedTime[i] = 1.0f;
	}
}

Server::~Server()
{
	/*for (unsigned int i = 0; i < MAX_PLAYERS; ++i)
		delete names[i];*/
	for (int i = 0; i < gameState->size(); ++i)
	{
		delete gameState[0][i].name;
	}


	delete clientStates;
	delete gameState;
	if (peer)
	peer->DestroyInstance(peer);
}

int Server::init(uint16_t port)
{
	printf("SERVER: test\n");

	shutdown = false;

	peer = RakNet::RakPeerInterface::GetInstance();

	SocketDescriptor sd(port, 0);
	sd.socketFamily = AF_INET;
	StartupResult res = peer->Startup(MAX_PLAYERS, &sd, 1);
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_UDP);
//	peer->GetSocket(peer->GetMyBoundAddress());
	if (res == SOCKET_PORT_ALREADY_IN_USE)
		return 0;
	
	printf("Starting the server.\n");
	peer->SetMaximumIncomingConnections(MAX_PLAYERS);

	printf("Server made at: %s\n", peer->GetMyBoundAddress().ToString());
	for (unsigned int i = 0; i < 8; ++i)
	{
		//names[i] = new char[8];
		clientIDs[i] = (UINT8)(i + 1);
	}

	objIDs[0].inUse = true;

	peer->SetOccasionalPing(true);

	message = nullptr;

	return 1;
}

int  Server::update()
{
	if (!peer)
		float temp = 0.0f;
	int result = 1;
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

			if (!npDec)
				--numPlayers;

			if (numPlayers == 1)
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
			char newMessage[50] = "Please welcome the new player.\n";
			//	memcpy(&newMessage[strlen(newMessage)], names[id], nameSizes[id]);
			//	memcpy(&newMessage[strlen(newMessage)], ".\n", 3);

			printf(newMessage);
			//sendMessage(newMessage, ID_SERVER_MESSAGE, true);
			sendMessage(numPlayers, ID_NEW_CLIENT, true);
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
			sendDisconnect(false);
			npDec = true;

			break;
		}
		case ID_INCOMING_PACKET:
		{
			recievePacket();
			++packRec;
			if (result != 3)
				result = 2;
			break;
		}
		case ID_INCOMING_INPUT:
		{
			recieveInput();
			result = 3;
			break;
		}

		case ID_CHANGE_TEAM_A:
		{
			BitStream bIn(packet->data, packet->length, false);
			bIn.IgnoreBytes(sizeof(MessageID));
			UINT8 tempID;
			bIn.Read(tempID);

			UINT8 objID, newID;

			if (tempID >= 4)
			{
				for (unsigned int i = 0; i < 4; ++i)
				{
					if (!objIDs[i].inUse)
					{
						objID = objIDs[i].id;
						objIDs[i].inUse = true;
						objIDs[tempID - 1].inUse = false;
						clientIDs[tempID - 1] = tempID;
						newID = i + 1;
						clientIDs[i] = 0;
						break;
					}
				}

				sendObjID(objID, newID);
			}


			break;
		}
		case ID_CHANGE_TEAM_B:
		{
			BitStream bIn(packet->data, packet->length, false);
			bIn.IgnoreBytes(sizeof(MessageID));
			UINT8 tempID;
			bIn.Read(tempID);

			UINT8 objID, newID;

			if (tempID < 5)
			{
				for (unsigned int i = 4; i < 8; ++i)
				{
					if (!objIDs[i].inUse)
					{
						objID = objIDs[i].id;
						objIDs[i].inUse = true;
						objIDs[tempID - 1].inUse = false;
						clientIDs[tempID - 1] = tempID;
						newID = i + 1;
						clientIDs[i] = 0;
						break;
					}
				}
				sendObjID(objID, newID);

			}

			break;
		}
		case ID_INCOMING_MESSAGE:
		{	ReceiveMessage();
		result = 4;
		break; }

		case ID_SPRINT_EMPTY:
		{
			BitStream bIn(packet->data, packet->length, false);
			bIn.IgnoreBytes(sizeof(MessageID));

			UINT8 id;
			bIn.Read(id);
			bIn.Read(gameState[0][id - 1].empty);

			break;
		}
		}

		if (!peer)
			float temp = 0.0f;
	}

	/*if (result == 2)
	{
		sendPackets();
	}*/
	npDec = false;
	return result;
}

void Server::stop()
{
	float time = 0.0f;
	shutdown = true;
	while (true)
	{
		time += 0.001f;
		sendDisconnect(true);
		int result = update();
		if (result == 0 || numPlayers <= 1 || time >= 1.0f)
			break;
	}
	//RakPeerInterface::DestroyInstance(peer);
}

bool Server::Shutdown()
{
	printf("\nClosing server\n");

	sendMessage("", ID_SERVER_CLOSURE, true);
	if (numPlayers == 0)
	{
		//peer->CloseConnection(peer->GetMyGUID(), false, '\000', IMMEDIATE_PRIORITY);
		peer->Shutdown(100);
		DataStructures::List<RakNetSocket2*> socs;
		peer->GetSockets(socs);
		peer->ReleaseSockets(socs);
		//closesocket(serverSocket);
		shutdown = true;
		return true;
	}

	delete message;

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
void Server::sendMessage(UINT8 message, GameMessages ID, bool broadcast)
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
	//bsIn.Read(names[clientid - 1], length);

	//nameSizes[clientid - 1] = length;
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
		if (clientIDs[i] != 0)
		{
			newID = clientIDs[i];
			clientIDs[i] = 0;
			objIDs[i].inUse = true;
			break;
		}
	}

	bsOut.Write(newID);
	bsOut.Write(serverObjs);

	peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}

void Server::sendObjID(UINT8 id, UINT8 newID)
{
	BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_CLIENT_OBJ);

	bsOut.Write(newID);
	bsOut.Write(id);
	bsOut.Write(serverObjs);

	peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}

void Server::unregisterClient()
{
	
	BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));

	UINT8 leavingID;
	bsIn.Read(leavingID);
	//int size = nameSizes[leavingID - 1];

	clientIDs[leavingID - 1] = leavingID;
	char message[30] = "A player has left the lobby.\n";
	//memcpy(&message[0], names[leavingID - 1], size);
	//memcpy(&message[size], "A player has left the lobby.\n", strlen("A player has left the lobby.\n"));
	sendMessage(message, ID_SERVER_MESSAGE, true);

	--numPlayers;
}

void Server::sendDisconnect(bool broadcast)
{
	sendMessage("", ID_REMOVE_CLIENT, broadcast);
}

void Server::recievePacket()
{
	BitStream bIn(packet->data, packet->length, false);
	bIn.IgnoreBytes(sizeof(MessageID));

	CLIENT_GAME_STATE tempState;

//	bIn.Read(tempState.timeStamp);
	bIn.Read(tempState.clientID);
//	bIn.Read(tempState.nameLength);
//	bIn.Read(tempState.animationName, (unsigned int)tempState.nameLength);
	bIn.Read(tempState.hasBall);
//	bIn.Read(tempState.world);
	bIn.Read(tempState.position);
	bIn.Read(tempState.rotation);
	bIn.Read(tempState.parentIndex);
	bIn.Read(tempState.animationIndex);

	clientStates[0][tempState.clientID] = tempState;

	lastState = (int)tempState.clientID;

}

void Server::recieveInput()
{
	BitStream bIn(packet->data, packet->length, false);
	bIn.IgnoreBytes(sizeof(MessageID));
	InputEventStruct tempState;
	//bIn.IgnoreBytes(sizeof(UINT8));
	//bIn.EndianSwapBytes(3, sizeof(KeyStates));
	//bIn.Read(tempState);

	bIn.Read(tempState.clientID);
	for (unsigned int i = 0; i < 256; ++i)
		bIn.Read(tempState.keyboard[i]);
	for (unsigned int i = 0; i < 256; ++i)
	bIn.Read(tempState.keyboardDown[i]);
	for (unsigned int i = 0; i < 256; ++i)
	bIn.Read(tempState.keyboardUp[i]);

	for (unsigned int i = 0; i < 3; ++i)
	bIn.Read(tempState.mouse[i]);
	for (unsigned int i = 0; i < 3; ++i)
	bIn.Read(tempState.mouseDown[i]);
	for (unsigned int i = 0; i < 3; ++i)
	bIn.Read(tempState.mouseUp[i]);
	bIn.Read(tempState.mouseX);
	bIn.Read(tempState.mouseY);
	bIn.Read(tempState.isServer);

	newInput[tempState.clientID - 1] = true;

	clientInput[tempState.clientID - 1] = tempState;
}

void Server::sendPackets()
{
	// send packet x8 to all clients
	if (serverObjs > 0 && gameState[0][0].scene == 2)
	{

		if (!peer)
			float temp = 0.0f;

		packRec = 0;
		BitStream bOut;
		bOut.Write((MessageID)ID_INCOMING_PACKET);
		bOut.Write((UINT8)serverObjs);

		for (unsigned int i = 0; i < (unsigned int)serverObjs; ++i)
		{
			//bOut.Write(GetTime());
			bOut.Write(clientStates[0][i].clientID);
			//bOut.Write(clientStates[i].nameLength);
			//bOut.Write(clientStates[i].animationName, (unsigned int)clientStates[i].nameLength);
			bOut.Write(clientStates[0][i].hasBall);
			//bOut.Write(clientStates[i].world);
			bOut.Write(clientStates[0][i].position);
			bOut.Write(clientStates[0][i].rotation);
			bOut.Write(clientStates[0][i].parentIndex);
			bOut.Write(clientStates[0][i].animationIndex);
			bOut.Write(clientStates[0][i].otherIndex);
			bOut.Write(clientStates[0][i].transitionIndex);
			bOut.Write(clientStates[0][i].soundID);
			bOut.Write(clientStates[0][i].hasSound);
		}
		peer->Send(&bOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);
	}
}

void Server::ReceiveMessage()
{
	BitStream bIn(packet->data, packet->length, false);
	bIn.IgnoreBytes(sizeof(MessageID));

	bIn.Read(stride);

	delete message;
	message = nullptr;

	message = new char[stride];

	for (int i = 0; i < stride; ++i)
	{
		bIn.Read(message[i]);
	}
}

void Server::setStates(unsigned int index, bool hasBall, XMFLOAT3 pos, XMFLOAT3 rot, int parentIndex, int animIndex, int oIndex, float _dt, int transitionIndex, unsigned int soundID, bool hasSound)
{
	//if (serverObjs > 0) {
		//	memcpy(clientStates[index].animationName, animationName, length);
		//	clientStates[index].nameLength = length;
		clientStates[0][index].clientID = index;
		clientStates[0][index].hasBall = hasBall;
		//	clientStates[index].world = state->world;
		clientStates[0][index].position = pos;
		clientStates[0][index].rotation = rot;
		clientStates[0][index].parentIndex = parentIndex;
		clientStates[0][index].animationIndex = animIndex;
		clientStates[0][index].otherIndex = oIndex;
		clientStates[0][index].transitionIndex = transitionIndex;
		clientStates[0][index].soundID = soundID;
		clientStates[0][index].hasSound = hasSound;

	//}
}

void Server::sendState() 
{
	BitStream bOut;
	bOut.Write((MessageID)ID_INCOMING_STATE);
	bOut.Write(gameState[0][0].scene);
	bOut.Write(gameState[0][0].paused);
	bOut.Write(gameState[0][0].countdown);
	bOut.Write(gameState[0][0].floorPulse);
	bOut.Write(gameState[0][0].scoreA);
	bOut.Write(gameState[0][0].scoreB);
	bOut.Write(gameState[0][0].time);
	bOut.Write(gameState[0][0]._dt);

	for (unsigned int i = 0; i < MAX_PLAYERS; ++i)
	{
		bOut.Write(gameState[0][i].sprintA);
		bOut.Write(gameState[0][i].sprintD);
		bOut.Write(gameState[0][i].down);
		bOut.Write(gameState[0][i].score);
		bOut.Write(gameState[0][i].assists);
		bOut.Write(gameState[0][i].saves);
		bOut.Write(gameState[0][i].goals);

		UINT8 nameLength = (UINT8)strlen(gameState[0][i].name);
		bOut.Write(nameLength);
		bOut.Write(gameState[0][i].name, nameLength);
	}


	peer->Send(&bOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);
}

void Server::sendMessage(char * message, uint16_t stride, uint16_t messageID)
{
	BitStream bsOut;

	bsOut.Write((RakNet::MessageID)ID_INCOMING_MESSAGE);
	bsOut.Write(messageID);
	bsOut.Write(stride);

	for (uint16_t i = 0; i < stride; ++i)
	{
		bsOut.Write(message[i]);
	}

	peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);
}

void Server::StartGame()
{
	gameState[0][0].paused = false;
	sendMessage(UINT8(11), ID_START_GAME, true);
}


void Server::setObjIDs(UINT8 one, UINT8 two, UINT8 three, UINT8 four, UINT8 five, UINT8 six, UINT8 seven, UINT8 eight)
{
	objIDs[0].id = one;
	objIDs[1].id = two;
	objIDs[2].id = three;
	objIDs[3].id = four;
	objIDs[4].id = five;
	objIDs[5].id = six;
	objIDs[6].id = seven;
	objIDs[7].id = eight;
}

void Server::SendScored(char * name, UINT8 length)
{
	BitStream out;
	out.Write((MessageID)ID_SOMEONE_SCORED);
	out.Write(length);
	out.Write(name, length);
	peer->Send(&out, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);
}

void Server::SendPowerUps()
{
	BitStream out;
	out.Write((MessageID)ID_SPAWN_POWERUP);
	//UINT8 amount = 6;
	//out.Write(amount);

	for (int i = 0; i < 6; ++i)
	{
		if (pUp.isactive[i])
			out.Write((UINT8)1);
		else
			out.Write((UINT8)0);
		if (pUp.isactive[i])
			out.Write(pUp.positions[i]);
	}
	float temp = 0.0f;

	peer->Send(&out, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);

//	pUp.positions.clear();
//	pUp.newindices.clear();
}

void Server::SendElapsedTime()
{
	BitStream out;
	out.Write((MessageID)ID_POWERUP_TIME);
	//UINT8 amount = 6;
	for (int i = 0; i < 6; ++i)
	{
		out.Write(pUp.elapsedTime[i]);
	}

	peer->Send(&out, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);

}

void Server::SendRemoved()
{

	BitStream out;
	out.Write((MessageID)ID_REMOVE_POWERUP);

	out.Write((UINT8)pUp.id);
	out.Write((UINT8)pUp.removeindices);
	pUp.isactive[pUp.removeindices] = false;
	pUp.positions[pUp.removeindices] = { 0,0,0 };

	peer->Send(&out, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);


}