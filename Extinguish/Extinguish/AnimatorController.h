#pragma once
#include <vector>
#include <string>
#include "Component.h"

class State;
class Parameter;
class Blender;
class Transition;
class Renderer;

class AnimatorController : public Component
{
private:
	Blender* blender;
	Renderer* renderer;
	std::vector<State*> states;
	std::vector<Parameter*> parameters;
	unsigned int currentState;

public:
	AnimatorController();
	~AnimatorController();

	//basic
	void Init(std::string animationSetName, unsigned int curStateIndex, std::string curAnimName);
	void Update(float dt) override;
	
	//misc//
	//void CreateAndAddState(std::string animName, bool doLoop, float animSpeed);
	void AddState(State* state);
	void AddParameter(Parameter* parameter);
	void TransitionTo(Transition* transition);

	//getters//
	Blender* GetBlender();

	//setters//
	void SetCurrentState(unsigned int curState);
	void SetCurrentState(State* curState);
};