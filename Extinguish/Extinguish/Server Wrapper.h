#pragma once

#include <vector>
#include "Server.h"
#include "..\ShaderStructures\ShaderStructures.h"
#include "EventDispatcher.h"
#include "InputDownEvent.h"

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
			//	memcpy(&states[i]->animationName, newServer.getState(i)->animationName, newServer.getState(i)->nameLength);
				states[i]->clientID = newServer.getState(i)->clientID;
				states[i]->hasBall = newServer.getState(i)->hasBall;
		//		states[i]->nameLength = newServer.getState(i)->nameLength;
				states[i]->position = newServer.getState(i)->position;
				states[i]->rotation = newServer.getState(i)->rotation;
			}
		}
		if (result == 3)
		{
			InputDownEvent* inputEvent = new InputDownEvent();

			for (unsigned int i = 0; i < 4; ++i)
			{
				InputEventStruct * tempEvent = (InputEventStruct*)newServer.getInputEvent(i);

				EventDispatcher::GetSingleton()->Dispatch(inputEvent);
			}

			delete inputEvent;
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
			newServer.setStates(i, gameStates[i]->hasBall, gameStates[i]->position, gameStates[i]->rotation);
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

	/*char * getInput()
	{
		return newServer.
	}*/
};