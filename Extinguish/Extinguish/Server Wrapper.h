#pragma once

#include <vector>
#include "Server.h"
#include "..\ShaderStructures\ShaderStructures.h"
#include "EventDispatcher.h"
#include "InputDownEvent.h"
#include "InputManager.h"

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

		if (result == 3)
		{
			InputDownEvent* inputEvent = new InputDownEvent();

			for (unsigned int i = 0; i < 4; ++i)
			{
				if (newServer.isInput(i))
				{
					InputEventStruct * tempEvent = new InputEventStruct;

					tempEvent->clientID = newServer.getInputEvent(i)->clientID;
					memcpy(tempEvent->keyboard, newServer.getInputEvent(i)->keyboard, 256);
					memcpy(tempEvent->keyboardUp, newServer.getInputEvent(i)->keyboardUp, 256);
					memcpy(tempEvent->keyboardDown, newServer.getInputEvent(i)->keyboardDown, 256);
					memcpy(tempEvent->mouse, newServer.getInputEvent(i)->mouse, 3);
					memcpy(tempEvent->mouseDown, newServer.getInputEvent(i)->mouseDown, 3);
					memcpy(tempEvent->mouseUp, newServer.getInputEvent(i)->mouseUp, 3);
					tempEvent->mouseX = newServer.getInputEvent(i)->mouseX;
					tempEvent->mouseY = newServer.getInputEvent(i)->mouseY;
					tempEvent->isServer = newServer.getInputEvent(i)->isServer;

					inputEvent->GetInput()->Init(tempEvent->keyboard, tempEvent->keyboardDown, tempEvent->keyboardUp, tempEvent->mouse, tempEvent->mouseDown, tempEvent->mouseUp, tempEvent->mouseX, tempEvent->mouseY);
					inputEvent->SetID(tempEvent->clientID);
					inputEvent->SetIsServer(tempEvent->isServer);

					EventDispatcher::GetSingleton()->Dispatch(inputEvent);

					delete tempEvent;
				}
			}

			newServer.resetInput();
			
			delete inputEvent;
			result = 2;
		}

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

			newServer.sendPackets();
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