#pragma once

#include "Server.h"

class ServerWrapper
{
private:
	Server newServer;
	bool shutdown = false;
	bool noPeer = false;
public:
	~ServerWrapper() { 
		//serverClient.stop();
	//newServer.stop();
	}
	int init(char * address, UINT16 port)
	{
		
		return newServer.init(port);
	}
	int run()
	{
		if (shutdown && !noPeer)
			noPeer = newServer.Shutdown();
		return newServer.update();
	}
	void stop()
	{
		noPeer = newServer.Shutdown();
		shutdown = true;
	}
};