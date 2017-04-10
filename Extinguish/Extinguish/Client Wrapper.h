#pragma once

#include <stdio.h>

#include "Client.h"
#include "..\ShaderStructures\ShaderStructures.h"
#include "InputManager.h"
#include "InputDownEvent.h"
#include "Game.h"
#include "ResourceManager.h"

class ClientWrapper
{
private:
	Client newClient;
	int id = 1;
	bool isMultiplayer = false;
	bool isServer = false;
public:
	~ClientWrapper()
	{
		if (isMultiplayer && !isServer)
			newClient.stop();
	}
	int init(char* address, UINT16 port)
	{
		printf("init client\n");
		isMultiplayer = true;
		return newClient.init(address, port);
	}
	void setIsServer() { isServer = true; }
	int run()
	{
		int result = newClient.run();
		int a = 0, b = 0, c = 0;
		if (newClient.hasPackets())
			a = 1;
		if (newClient.hasScored())
			b = 1;
		if (newClient.hasState())
			c = 1;

		printf("Results | Packet %i | Scored %i | State %i\n", a, b, c);
		if (ResourceManager::GetSingleton()->IsPaused() != newClient.getPaused())
		ResourceManager::GetSingleton()->SetPaused(newClient.getPaused());
		return result;
	}
	void sendStop()
	{
		newClient.sendStop();
	}
	void sendMessage(char * message)
	{
		newClient.sendMessage(message);
	}

	void sendMessage(char * message, uint16_t stride)
	{
		newClient.sendMessage(message, stride);
	}

	void sendPacket()
	{
		newClient.sendPacket();
	}
	void sendInput(InputDownEvent* inputEvent)
	{

		//Client::InputEventStruct * temp = new Client::InputEventStruct;

		//temp->up = 1;
		//temp->down = 2;
		//temp->left = 3;
		//temp->right = 4;
		InputManager* input = inputEvent->GetInput();

		newClient.sendInput(input->GetKeyboard(),
			input->GetKeyboardDown(),
			input->GetKeyboardUp(),
			input->GetMouse(),
			input->GetMouseDown(),
			input->GetMouseUp(),
			input->GetMouseX(),
			input->GetMouseY(),
			//inputEvent->GetID(), 
			inputEvent->IsServer());
		//delete temp;
	}

	unsigned int getID()
	{
		id = newClient.getID();
		//printf("Client ID: %i\n", id);
		return (unsigned int)id;
	}

	int getPacketCount()
	{
		return newClient.getNumPackets();
	}

	bool hasBall(unsigned int index)
	{
		return newClient.hasBall(index);
	}

	INT8 GetParentIndex(unsigned int index)
	{
		return newClient.GetParentIndex(index);
	}

	INT8 GetAnimationIndex(unsigned int index)
	{
		return newClient.GetAnimationIndex(index);
	}

	INT8 GetTransitionIndex(unsigned int index)
	{
		return newClient.GetTransitionIndex(index);
	}

	UINT32 GetSoundID(unsigned int index)
	{
		return newClient.GetSoundID(index);
	}

	bool HasSound(unsigned int index)
	{
		return newClient.HasSound(index);
	}

	XMFLOAT3 getLocation(unsigned int index)
	{
		return newClient.getLocation(index);
	}

	char* getMessage()
	{
		return newClient.getMessage();
	}

	uint16_t GetMessageID()
	{
		return newClient.GetMessageID();
	}

	uint16_t GetStride()
	{
		return newClient.GetStride();
	}

	void setLocation(XMFLOAT3 loc)
	{
		newClient.setLocation(loc);
	}

	XMFLOAT3 getRotation(unsigned int index)
	{
		return newClient.getRotation(index);
	}

	void setRotation(XMFLOAT3 rot)
	{
		newClient.setRotation(rot);
	}

	void changeTeamA()
	{
		newClient.changeTeam(0);
	}

	void changeTeamB()
	{
		newClient.changeTeam(1);
	}

	int getFloorState(unsigned int i) { return newClient.getFloorState(i); }

	int getScoreA() { return newClient.getScoreA(); }
	int getScoreB() { return newClient.getScoreB(); }
	float getTime() { return newClient.getTime(); }

	UINT8 getObjID() { return newClient.getObjID(); }

	UINT8 getNumClients() { return newClient.getNumClients(); }

	bool getMeterActive() { return newClient.getMeterActive(); }
	bool getMeterDrain() { return newClient.getMeterDrain(); }
	bool getMeterDown() { return newClient.getMeterDown(); }
	void sendEmpty(bool toggle) { newClient.sendEmpty(toggle); }

	int stateSize() { return newClient.stateSize(); }
	float getDT() { return newClient.getDT(); }

	char* updateScoreboard(unsigned int i, unsigned int &score, unsigned int &assists, unsigned int &saves, unsigned int &goals, char * name)
	{
		return newClient.getScoreboard(i, score, assists, saves, goals, name);
	}

	unsigned int getScene() { return newClient.getScene(); }
	bool getCountdown() { return newClient.getCountdown(); }

	const char * getScorer() { return newClient.getScoreName(); }

	bool hasPackets() { return newClient.hasPackets(); }
	bool hasState() { return newClient.hasState(); }
	bool hasScored() { return newClient.hasScored(); }

	int GetPulse() { return newClient.getPulse(); }

	bool SpawnedPowerUps() { return newClient.SpawnedPowerUps(); }
	bool RemovedPowerUp() { return newClient.RemovedPowerUp(); }

	int SpawnedPowerUpAmount() { 
		return newClient.SpawnedPowerUpAmount(); }
	bool getSpawnedPowerUpActive(int i) { return newClient.getSpawnedPowerUpActive(i); }

	float3 getSpawnedPowerUpPos(int i) {
		XMFLOAT3 npos = newClient.GetSpawnedPowerUpPos(i);
		return { npos.x, npos.y, npos.z };
	}

	int RemovedPowerUpAmount() { return newClient.RemovedPowerUpAmount(); }
	int getRemovedPowerUpIndex(int i) { return newClient.getRemovedPowerUpIndex(i); }
	int getRemovedPlayerID(int i) { return newClient.getRemovedPlayerID(i); }
	float powerUpTime(int i) {return newClient.getPowerUpTime(i);}
};