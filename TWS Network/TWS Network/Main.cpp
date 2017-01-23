
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>

#include "Client Wrapper.h"
#include "Server Wrapper.h"
#include "RakNet\Gets.h"



#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

using namespace std;
bool input = false;

int main(void)
{
	printf("Welcome. Would you like to [h]ost or [j]oin a server?\n");
	
	char t = 0;

	/*while (t == 0)
		t = _getch();*/

	bool isServer = true;
	ServerWrapper newServer;
		ClientWrapper newClient;
	if (newServer.init("127.0.0.1", 60000) == 1)
	{
		isServer = true;
		newClient.init("127.0.0.1", 60001);
	}
	else
	{
		
		newClient.init("127.0.0.1", 60001);
	}
	
	if (!isServer)
	{
		//newClient.init("127.0.0.1", 60001);
	
		while (true)
		{
			if (_kbhit())
			{
				char message[255];
				Gets(message, sizeof(message));

				if (strcmp(message, "exit") == 0)
				{
					newClient.sendStop();
					//break;
				}

				else if (strcmp(message, "talk") == 0)
				{
					char * temp;
					printf("Input message: ");
					char newMessage[255];
					temp = Gets(newMessage, sizeof(newMessage));
					
					newClient.sendMessage(newMessage);
				}
				else if (strcmp(message, "send") == 0)
				{
					newClient.sendPacket();
				}
				else if (strcmp(message, "move") == 0)
				{
					newClient.sendInput();
				}
			}

			if (newClient.run() == 0)
			{
				//newClient.sendStop();
				break;
			}
		}
	}
	else
	{
	//	newServer.init("127.0.0.1", 60000);
	
		while (true)
		{
			if (_kbhit())
			{
				char message[255];
				Gets(message, sizeof(message));

				if (strcmp(message, "exit") == 0)
				{
					newServer.stop();
				}
			}

			if (newServer.run() == 0)
			{
				break;
			}
		}

	}
}