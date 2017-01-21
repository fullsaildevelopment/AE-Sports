#pragma once

#include <stdio.h>

#include "Client.h"

class ClientWrapper
{
private:
	Client newClient;

public:
	~ClientWrapper()
	{
		//newClient.stop();
	}
	int init(char* address, UINT16 port)
	{
		return newClient.init(address, port);
	}
	int run() 
	{
		return newClient.run();
	}
	void sendStop()
	{
		newClient.sendStop();
	}
	void sendMessage(char * message)
	{
		newClient.sendMessage(message);
	}
	void sendPacket()
	{
		newClient.sendPacket();
	}
	void sendInput()
	{
		
		Client::KeyStates * temp = new Client::KeyStates;
		
		temp->up = 1;
		temp->down = 2;
		temp->left = 3;
		temp->right = 4;
		newClient.sendInput(temp);
		delete temp;
	}
};