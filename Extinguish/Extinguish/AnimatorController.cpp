#include "AnimatorController.h"
#include "State.h"
#include "Parameter.h"
#include "Blender.h"
#include "Transition.h"

AnimatorController::AnimatorController()
{
	blender = new Blender();
}

AnimatorController::~AnimatorController()
{
	delete blender;
}

//basic//
void AnimatorController::Init(std::string animationSetName, unsigned int curStateIndex, std::string curAnimName)
{
	blender->SetAnimationSet(ResourceManager::GetSingleton()->GetAnimationSet(animationSetName));
	blender->Init(curAnimName, "");
}

void AnimatorController::Update(float dt)
{
	states[currentState]->Update(dt);

	blender->Update(dt, 0);
}

//misc//
void AnimatorController::AddState(State* state)
{
	states.push_back(state);
}

void AnimatorController::AddParameter(Parameter* parameter)
{
	parameters.push_back(parameter);
}

void AnimatorController::TransitionTo(Transition* transition)
{
	BlendInfo info;
	info.totalBlendTime = transition->GetTransitionDuration();
	blender->SetBlendInfo(info);
	blender->GetNextInterpolator()->SetAnimation(transition->GetToState()->GetAnimation());
	blender->GetNextInterpolator()->SetSpeed(transition->GetToState()->GetSpeed());
	blender->GetNextInterpolator()->SetIsLoop(transition->GetToState()->DoesItLoop());
}

//setters//
void AnimatorController::SetCurrentState(unsigned int curState)
{
	currentState = curState;
}

void AnimatorController::SetCurrentState(State* curState)
{
	for (int i = 0; i < states.size(); ++i)
	{
		if (curState == states[i])
		{
			currentState = i;
			break;
		}
	}
}