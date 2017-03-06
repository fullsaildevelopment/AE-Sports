#pragma once

#include <stdio.h>

#include "Client.h"
#include "..\ShaderStructures\ShaderStructures.h"
#include "InputManager.h"

class ClientWrapper
{
private:
	Client newClient;
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
		isMultiplayer = true;
		return newClient.init(address, port);
	}
	void setIsServer() { isServer = true; }
	int run() 
	{
		return newClient.run();
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
		return (unsigned int)newClient.getID();
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

//	float getTime() { return newClient.getTime(); }
};