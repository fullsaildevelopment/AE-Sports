
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>

#include "Client Wrapper.h"
#include "Server Wrapper.h"

//#include "RakNet\WindowsIncludes.h"

using namespace std;

int main(void)
{
	printf("Welcome. Would you like to [h]ost or [j]oin a server?\n");
	
	char t = 0;

	while (t == 0)
		t = getch();
	
	if (t == 'j' || t == 'J')
	{
		ClientWrapper newClient;
		newClient.init("127.0.0.1", 60000);
	
		while (true)
		{
			if (newClient.run() == 0)
				break;
		}
	}
	else
	{
		ServerWrapper newServer;
		newServer.init("127.0.0.1", 60000);
	
		while (true)
		{
			if (newServer.run() == 0)
				break;
		}
	}
}