#include "AnimatorController.h"
#include "State.h"
#include "Parameter.h"
#include "Blender.h"
#include "Transition.h"

void AnimatorController::Update(float dt)
{
	Transition* transition = states[currentState]->Update(dt);

	if (transition)
	{
		BlendInfo info;
		info.totalBlendTime = transition->GetTransitionDuration();
		blender->SetBlendInfo(info);
		blender->GetNextInterpolator()->SetAnimation(transition->GetToState()->GetAnimation());
	}

	blender->Update(dt, 0);
}

void AnimatorController::AddParameter(Parameter* parameter)
{
	parameters.push_back(parameter);
}

void AnimatorController::SetCurrentState(unsigned int curState)
{
	currentState = curState;
}