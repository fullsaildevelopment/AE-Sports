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
	std::vector<Parameter*> conditions;

	bool hasExitTime;
	float exitTime;
	float transitionDuration;
	float timer;
	bool doTransition;
public:
	Transition();
	~Transition();

	//basic
	void Init(State* curState, State* nextState, float exitTime, float transitionDuration);
	bool Update(float dt);

	//misc//
	void AddCondition(Parameter* parameter);
	//void AddCondition(Parameter* parameter);

	//getters//
	float GetTransitionDuration();
	State* GetFromState();
	State* GetToState();

	//setters//
	//void SetTransitionDuration(float duration);

};