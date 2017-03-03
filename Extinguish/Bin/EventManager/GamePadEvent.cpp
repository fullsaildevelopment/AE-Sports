#include "GamePadEvent.h"

using namespace DirectX;

void GamePadEvent::Init()
{
	//nothing
}

void GamePadEvent::Init(GamePad::State* state, int id)
{
	gamePadState = state;
	clientID = id;
}

//getters//
GamePad::State* GamePadEvent::GetState()
{
	return gamePadState;
}

int GamePadEvent::GetClientID()
{
	return clientID;
}