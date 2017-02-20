#include "Server.h"

//Server::CLIENT_GAME_STATE * Server::clientStates = new CLIENT_GAME_STATE[8];
//Server::CLIENT_GAME_STATE * Server::clientStates =  new CLIENT_GAME_STATE[30];
//Server::GAME_STATE * Server::gameState = new GAME_STATE();

//std::vector<Server::CLIENT_GAME_STATE> clientStates;
//std::vector<Server::GAME_STATE> gameState;


void Server::setObjectCount(int count) { 
	serverObjs = count;
//	clientStates = new CLIENT_GAME_STATE[serverObjs]();
}

Server::Server()
{
	clientStates = new std::vector<CLIENT_GAME_STATE>();
	gameState = new std::vector<GAME_STATE>();
}

Server::~Server()
{
	for (unsigned int i = 0; i < MAX_PLAYERS; ++i)
		delete names[i];

	delete clientStates;
	delete gameState;
//	delete[] clientStates;
	if (peer)
	peer->DestroyInstance(peer);
}

int Server::init(uint16_t port)
{
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
	for (unsigned int i = 0; i < MAX_PLAYERS; ++i)
	{
		names[i] = new char[8];
		openIDs[i] = i + 1;
	}

	peer->SetOccasionalPing(true);

	return 1;
}

int  Server::update()
{
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
			char newMessage[50] = "Please welcome the new player, ";
			memcpy(&newMessage[strlen(newMessage)], names[id], nameSizes[id]);
			memcpy(&newMessage[strlen(newMessage)], ".\n", 3);

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
			//sendPackets(); // for testing purposes
			++packRec;
			if (result != 3)
				result = 2;
			break;
		}
		case ID_INCOMING_INPUT:
		{
			recieveInput();
			result = 3;
			// and do something with it
			break;
		}
		}
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
	shutdown = true;
	while (true)
	{
		sendDisconnect(true);
		int result = update();
		if (result == 0 || numPlayers <= 1)
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
	bsOut.Write(serverObjs);

	peer->Send(&bsOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
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
	//if (packRec >= numPlayers)
	{
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

void Server::setStates(unsigned int index, bool hasBall, XMFLOAT3 pos, XMFLOAT3 rot, int parentIndex, int animIndex, int oIndex, int transitionIndex, unsigned int soundID, bool hasSound)
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

	bOut.Write(gameState[0][0].scoreA);
	bOut.Write(gameState[0][0].scoreB);
	bOut.Write(gameState[0][0].time);

	peer->Send(&bOut, IMMEDIATE_PRIORITY, RELIABLE_ORDERED, 0, peer->GetMyBoundAddress(), true);
}

void Server::StartGame()
{
	sendMessage(UINT8(0), ID_START_GAME, true);
}