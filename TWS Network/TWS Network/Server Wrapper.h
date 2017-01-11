#pragma once

#include "Server.h"

class ServerWrapper
{
private:
	Server newServer;
	Client serverClient;
public:
	int init(char * address, UINT16 port)
	{
		newServer.init(port);
		return serverClient.init(address, port);
	}
	int run()
	{
		newServer.update();
		return serverClient.run();
	}
};