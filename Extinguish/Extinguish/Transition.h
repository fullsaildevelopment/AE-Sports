#pragma once
#include <vector>

class State;
class Parameter;
class Trigger;

//a transition is the time when we go from one state to another while blending between them
class Transition
{
private:
	State* from;
	State* to;
	std::vector<Parameter*> conditions;

	//exit time will make transition happen regardless of conditions
	bool hasExitTime;
	float exitTime;

	//how long will we transition (aka blend between states)
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
	void SetDoTransition(bool toggle);
	void SetTimer(float time);
	//void SetTransitionDuration(float duration);

};