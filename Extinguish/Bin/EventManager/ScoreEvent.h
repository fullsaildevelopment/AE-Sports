#pragma once
#include "Event.h"

class ScoreEvent : public Event
{
	int team;
public:
	void Init() {};
	int GetTeam() { return team; };
	void SetTeam(int t) { team = t; };
};