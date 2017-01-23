#pragma once

#include <vector>
#include "Server.h"
#include "..\ShaderStructures\ShaderStructures.h"
class ServerWrapper
{
private:
	Server newServer;
	bool shutdown = false;
	bool noPeer = false;
	std::vector<GameState*> states;

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

	void SetGameStates(std::vector<GameState*> gameStates)
	{
		states = gameStates;
	}

	std::vector<GameState*> getStates()
	{
		return states;
	}

	XMFLOAT4X4 getLocation(unsigned int index)
	{
		return newServer.getLocation(index);
	}
};