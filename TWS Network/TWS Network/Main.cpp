
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>

#include "Client Wrapper.h"
#include "Server Wrapper.h"
#include "RakNet\Gets.h"

using namespace std;

int main(void)
{
	printf("Welcome. Would you like to [h]ost or [j]oin a server?\n");
	
	char t = 0;

	while (t == 0)
		t = _getch();
	
	if (t == 'j' || t == 'J')
	{
		ClientWrapper newClient;
		newClient.init("10.63.6.159", 60001);
	
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
		ServerWrapper newServer;
		newServer.init("127.0.0.1", 60000);
	
		while (true)
		{
			if (newServer.run() == 0)
			{
				newServer.stop();
				break;
			}
		}
	}
}