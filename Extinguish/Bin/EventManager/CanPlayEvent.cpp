#include "CanPlayEvent.h"

CanPlayEvent::CanPlayEvent(bool canPlay)
{
	SetCanPlay(canPlay);
}

//basic
void CanPlayEvent::Init()
{

}

void CanPlayEvent::Init(bool canPlay)
{
	SetCanPlay(canPlay);
}

//getter
bool CanPlayEvent::CanPlay()
{
	return canPlay;
}

//setter
void CanPlayEvent::SetCanPlay(bool canPlay)
{
	this->canPlay = canPlay;
}