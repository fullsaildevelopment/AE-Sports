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
	unsigned int objCount = 0;
	bool isMultiplayer = false;
	bool isServer = false;

public:
	~ServerWrapper() { 
		if (isMultiplayer && isServer)
			newServer.stop();
	}

	int init(char * address, UINT16 port)
	{
		isServer = true;
		isMultiplayer = true;
		noPeer = false;
		shutdown = false;
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
			InputManager* input = new InputManager();

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

					input->Init(tempEvent->keyboard, tempEvent->keyboardDown, tempEvent->keyboardUp, tempEvent->mouse, tempEvent->mouseDown, tempEvent->mouseUp, tempEvent->mouseX, tempEvent->mouseY);
					inputEvent->SetInput(input);
					inputEvent->SetID(tempEvent->clientID);
					inputEvent->SetIsServer(tempEvent->isServer);

					EventDispatcher::GetSingleton()->DispatchTo(inputEvent, "Game");

					delete tempEvent;
				}
			}

			newServer.resetInput();
			
			delete inputEvent;
			delete input;
			//result = 2;
			newServer.sendPackets();
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
		newServer.setStateSize((unsigned int)gameStates.size());

		for (unsigned int i = 0; i < states.size(); ++i)
		{
			newServer.setStates(i, gameStates[i]->hasBall, gameStates[i]->position, gameStates[i]->rotation, gameStates[i]->parentIndex, gameStates[i]->animationIndex, gameStates[i]->otherIndex, gameStates[i]->transitionIndex, gameStates[i]->soundID, gameStates[i]->hasSound);
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

	char* getMessage()
	{
		return newServer.getMessage();
	}

	uint16_t GetStride()
	{
		return newServer.GetStride();
	}

	void setObjCount(int count)
	{
		objCount = count;
		newServer.setObjectCount(count);
	}

	int getNewState()
	{
		return newServer.getNewState();
	}

	unsigned int getTotalPlayers()
	{
		return newServer.getPlayerCount();
	}

	/*char * getInput()
	{
		return newServer.
	}*/

	unsigned int getObjCount() { return objCount; }

	void setScores(int scoreA, int scoreB) { newServer.setScores(scoreA, scoreB); }
	void setTime(float time) { newServer.setTime(time); }

	void sendGameState() { newServer.sendState(); }
	void StartGame() { newServer.StartGame(); }

	void setObjIDs(UINT8 one, UINT8 two, UINT8 three, UINT8 four, UINT8 five, UINT8 six, UINT8 seven, UINT8 eight) { newServer.setObjIDs(one, two, three, four, five, six, seven, eight); }
};