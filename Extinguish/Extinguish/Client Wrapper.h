#pragma once

#include <stdio.h>

#include "Client.h"
#include "..\ShaderStructures\ShaderStructures.h"
#include "InputManager.h"

class ClientWrapper
{
private:
	Client newClient;
public:
	~ClientWrapper()
	{
		//newClient.stop();
	}
	int init(char* address, UINT16 port)
	{
		return newClient.init(address, port);
	}
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
			inputEvent->GetID(), 
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


	int getFloorState(unsigned int i) { return newClient.getFloorState(i); }

	int getScoreA() { return newClient.getScoreA(); }
	int getScoreB() { return newClient.getScoreB(); }
	float getTime() { return newClient.getTime(); }
};