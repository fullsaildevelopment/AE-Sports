#include "AnimatorController.h"
#include "State.h"
#include "Parameter.h"
#include "Blender.h"
#include "Transition.h"
#include "Renderer.h"
#include "Trigger.h"
#include "HashString.h"

AnimatorController::AnimatorController()
{
	blender = new Blender();
	parametersTable = new HashString();
	statesTable = new HashString();
}

AnimatorController::~AnimatorController()
{
	delete blender;
	delete parametersTable;
	delete statesTable;

	for (int i = 0; i < states.size(); ++i)
	{
		delete states[i];
	}
}

//basic//
void AnimatorController::Init(std::string animationSetName, unsigned int curStateIndex, std::string curAnimName)
{
	//cache
	renderer = GetGameObject()->GetComponent<Renderer>();

	//set up blender
	blender->SetAnimationSet(ResourceManager::GetSingleton()->GetAnimationSet(animationSetName));
	blender->Init(curAnimName, "");
	
	currentState = curStateIndex;
	//blender->GetCurInterpolator()->SetIsLoop(states[currentState]->DoesItLoop());
	//blender->GetCurInterpolator()->SetSpeed(states[currentState]->GetSpeed());
}

void AnimatorController::Update(float dt)
{
	//check states' transitions
	states[currentState]->Update(dt);

	//do interpolation stuff
	blender->Update(dt, 0);

	//send renderer the offsets from newly updated blender
	renderer->SetBoneOffsets(blender->GetBoneOffsets());
}

void AnimatorController::UpdateCurAnimatorsLoopAndSpeed()
{
	blender->GetCurInterpolator()->SetIsLoop(states[currentState]->DoesItLoop());
	blender->GetCurInterpolator()->SetSpeed(states[currentState]->GetSpeed());
}

//misc//
//void AnimatorController::CreateAndAddState(std::string animName, bool doLoop, float animSpeed)
//{
//	State* state = new State();
//	state->Init(this, blender->GetAnimationSet()->GetAnimation(animName), doLoop, animSpeed);
//
//	states.push_back(state);
//}

void AnimatorController::AddState(State* state)
{
	states.push_back(state);
	statesTable->Insert(state->GetName());
}

void AnimatorController::AddParameter(Parameter* parameter)
{
	parameters.push_back(parameter);
	parametersTable->Insert(parameter->GetName());
}

//void AnimatorController::AddTrigger(std::string name, bool toggle)
//{
//	Param::Trigger* trigger = new Param::Trigger();
//
//	trigger->SetName(name);
//	trigger->SetTrigger(toggle);
//
//	parameters.push_back(trigger);
//}

void AnimatorController::TransitionTo(Transition* transition)
{
	BlendInfo info;
	info.totalBlendTime = transition->GetTransitionDuration();
	blender->SetBlendInfo(info);
	blender->GetNextInterpolator()->SetAnimation(transition->GetToState()->GetAnimation());
	blender->GetNextInterpolator()->SetSpeed(transition->GetToState()->GetSpeed());
	blender->GetNextInterpolator()->SetIsLoop(transition->GetToState()->DoesItLoop());
}

//getters//
Blender* AnimatorController::GetBlender()
{
	return blender;
}

State* AnimatorController::GetState(std::string name)
{
	State* result = nullptr;
	int index = statesTable->GetKey(name);

	if (index != -1)
	{
		result = states[index];
	}

	return result;
}

State* AnimatorController::GetState(unsigned int index)
{
	return states[index];
}

unsigned int AnimatorController::GetCurrentStateIndex()
{
	return currentState;
}

Param::Trigger* AnimatorController::GetTrigger(std::string name)
{
	Param::Trigger* result = nullptr;
	int index = parametersTable->GetKey(name);

	if (index != -1)
	{
		result = dynamic_cast<Param::Trigger*>(parameters[index]);
	}

	return result;
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

void AnimatorController::SetTrigger(std::string name)
{
	int index = parametersTable->GetKey(name);

	if (index != -1)
	{
		Param::Trigger* trigger = (Param::Trigger*)parameters[index];

		if (trigger)
		{
			trigger->SetTrigger();
		}
	}
}