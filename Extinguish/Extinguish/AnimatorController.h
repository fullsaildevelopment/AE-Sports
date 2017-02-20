#pragma once
#include <vector>
#include <string>
#include "Component.h"

class State;
class Parameter;
class Blender;
class Transition;
class Renderer;
class HashString;
namespace Param
{
	class Trigger;
};

class AnimatorController : public Component
{
private:
	Blender* blender;
	Renderer* renderer;
	std::vector<State*> states;
	std::vector<Parameter*> parameters;
	unsigned int curStateIndex;
	int nextStateIndex;
	HashString* parametersTable;
	HashString* statesTable;
	//Transition* curTransition;
public:
	AnimatorController();
	~AnimatorController();

	//basic
	void Init(std::string animationSetName, unsigned int curStateIndex, std::string curAnimName);
	void FixedUpdate(float dt) override;
	void UpdateCurAnimatorsLoopAndSpeed();

	//misc//
	void AddState(State* state);
	void AddParameter(Parameter* parameter);
	void TransitionTo(Transition* transition);
	void TransitionTo(unsigned int stateIndex, unsigned int transitionIndex); //used by client in game.cpp
	//void CreateAndAddState(std::string animName, bool doLoop, float animSpeed);
	//void AddTrigger(std::string name, bool toggle);

	//getters//
	Blender* GetBlender();
	State* GetState(std::string name);
	State* GetState(int index);
	unsigned int GetCurrentStateIndex();
	Param::Trigger* GetTrigger(std::string name);
	int GetNextStateIndex();

	//setters//
	void SetCurrentState(unsigned int curState);
	void SetCurrentState(State* curState);
	void SetTrigger(std::string name);
};