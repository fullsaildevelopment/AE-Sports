#pragma once
#include <stdio.h>
#include <Windows.h>

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
};