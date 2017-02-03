#include "AnimatorController.h"
#include "State.h"
#include "Parameter.h"

void AnimatorController::Update(float dt)
{
	states[currentState]->Update();
}

void AnimatorController::AddParameter(Parameter* parameter)
{
	parameters.push_back(parameter);
}