#pragma once
#include <string>
#include "Event.h"

class LoadSceneEvent : public Event
{
private:
	std::string name;

public:
	//no need to implement
	void Init() override;
	void Init(std::string sceneName);

	//getter//
	std::string GetName();

	//setter//
	void SetName(std::string sceneName);
};