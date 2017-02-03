#pragma once
#include <vector>
#include "Component.h"

class State;
class Parameter;

class AnimatorController : public Component
{
private:
	std::vector<State*> states;
	std::vector<Parameter*> parameters;
	unsigned int currentState;

public:
	//basic
	void Update(float dt) override;
	
	//misc//
	void AddParameter(Parameter* parameter);
};