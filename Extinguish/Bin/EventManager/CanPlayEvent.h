#pragma once
#include "Event.h"

class CanPlayEvent : public Event
{
private:
	bool canPlay;

public:
	CanPlayEvent(bool canPlay);

	//basic
	void Init() override;
	void Init(bool canPlay);

	//getter
	bool CanPlay();

	//setter
	void SetCanPlay(bool canPlay);

};