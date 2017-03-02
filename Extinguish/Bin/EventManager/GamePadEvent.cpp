#include "GamePadEvent.h"

void GamePadEvent::Init()
{
	//nothing
}

void GamePadEvent::Init(void* state)
{
	gamePadState = state;
}

//getters//
void* GamePadEvent::GetState()
{
	return gamePadState;
}