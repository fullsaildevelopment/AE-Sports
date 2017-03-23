#pragma once
#include <string>
#include "Event.h"

class ScoreEvent : public Event
{
	int team;
	std::string playerName;

public:
	void Init() {};
	int GetTeam() { return team; };
	std::string& GetPlayerName() { return playerName; }
	void SetTeam(int t) { team = t; };
	void SetPlayerName(std::string name) { playerName = name; }
};