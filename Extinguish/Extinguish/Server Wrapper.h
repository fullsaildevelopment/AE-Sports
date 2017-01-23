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

		newServer.sendPackets();
		int result = newServer.update();
		if (result == 2)
		{
			for (unsigned int i = 0; i < states.size(); ++i)
			{
				states[i] = (GameState*)newServer.getState(i);
			}
		}

		return result;
	}
	void stop()
	{
		noPeer = newServer.Shutdown();
		shutdown = true;
	}

	void SetGameStates(std::vector<GameState*> gameStates)
	{
		states = gameStates;

		for (unsigned int i = 0; i < states.size(); ++i)
		{
			newServer.setStates(i, gameStates[i]->clientID, gameStates[i]->animationName, gameStates[i]->nameLength, gameStates[i]->hasBall, gameStates[i]->position, gameStates[i]->rotation);
		}
	}

	void sendStates()
	{
		newServer.sendPackets();
	}

	std::vector<GameState*> getStates()
	{
		return states;
	}

	XMFLOAT3 getLocation(unsigned int index)
	{
		return newServer.getLocation(index);
	}

	XMFLOAT3 getRotation(unsigned int index)
	{
		return newServer.getRotation(index);
	}

	void setObjCount(int count)
	{
		newServer.setObjectCount(count);
	}

	int getNewState()
	{
		return newServer.getNewState();
	}
};