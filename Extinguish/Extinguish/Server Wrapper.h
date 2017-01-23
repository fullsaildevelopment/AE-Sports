#pragma once

#include "Server.h"
#include "..\ShaderStructures\ShaderStructures.h"
class ServerWrapper
{
private:
	Server newServer;
	bool shutdown = false;
	bool noPeer = false;
public:
	~ServerWrapper() { 
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

	GameState * getStates()
	{
		return NULL;
	}

	XMFLOAT4X4 getLocation(unsigned int index)
	{
		return newServer.getLocation(index);
	}
};