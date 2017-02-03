#pragma once
#include <vector>

class State;
class Parameter;
class Trigger;

class Transition
{
private:
	State* from;
	State* to;

	float exitTime;


	std::vector<Parameter*> conditions;
public:
	//basic
	void Update();

	//misc//
	void AddCondition(Parameter* parameter);
	//void AddCondition(Parameter* parameter);
};