#pragma once
#include <string>

class Parameter
{
private:
	std::string name;

public:
	virtual bool Check() = 0;

	//getters//
	std::string GetName();

	//setters//
	void SetName(std::string triggerName);
};