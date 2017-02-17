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

	for (int i = 0; i < parameters.size(); ++i)
	{
		delete parameters[i];
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
	
	curStateIndex = curStateIndex;
	nextStateIndex = -1;
	//curTransition = nullptr;
	//isTransitioning = false;
	//blender->GetCurInterpolator()->SetIsLoop(states[currentState]->DoesItLoop());
	//blender->GetCurInterpolator()->SetSpeed(states[currentState]->GetSpeed());
}

void AnimatorController::Update(float dt)
{
	//check states' transitions
	states[curStateIndex]->Update(dt);

	//do interpolation stuff
	blender->Update(dt, 0);

	//send renderer the offsets from newly updated blender
	renderer->SetBoneOffsets(blender->GetBoneOffsets());
}

void AnimatorController::UpdateCurAnimatorsLoopAndSpeed()
{
	blender->GetCurInterpolator()->SetIsLoop(states[curStateIndex]->DoesItLoop());
	blender->GetCurInterpolator()->SetSpeed(states[curStateIndex]->GetSpeed());
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
	
	nextStateIndex = statesTable->GetKey(transition->GetToState()->GetName());
	//curTransition = transition; // to be used by client
}

void AnimatorController::TransitionTo(unsigned int stateIndex, unsigned int transitionIndex)
{
	//cout << curStateIndex << " " << stateIndex << " " << nextStateIndex << endl;

	//if (stateIndex == 2)
	//{
	//	cout << "Breakpoint" << endl;
	//}

	if (stateIndex != curStateIndex) //don't do if it's already been done
	{
		State* curState = states[curStateIndex];
		
		if (transitionIndex < curState->GetTransitionsCount()) //crappy fix
		{
			Transition* transition = curState->GetTransition(transitionIndex);

			TransitionTo(transition);
			//State* nextState = transition->GetToState();

			//BlendInfo info;
			//info.totalBlendTime = transition->GetTransitionDuration();
			//blender->SetBlendInfo(info);
			//blender->GetNextInterpolator()->SetAnimation(nextState->GetAnimation());
			//blender->GetNextInterpolator()->SetSpeed(nextState->GetSpeed());
			//blender->GetNextInterpolator()->SetIsLoop(nextState->DoesItLoop());

		//	curStateIndex = stateIndex;
			nextStateIndex = stateIndex;
			transition->SetDoTransition(true);
			transition->SetTimer(0.0f);
		}
	}
	//else
	//{
	//	cout << "Breakpoint" << endl;
	//}

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

State* AnimatorController::GetState(int index)
{
	State* result = nullptr;

	if (index >= 0)
	{
		result = states[index];
	}

	return result;
}

unsigned int AnimatorController::GetCurrentStateIndex()
{
	return curStateIndex;
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

int AnimatorController::GetNextStateIndex()
{
	return nextStateIndex;
}

//setters//
//to be used by the client to set the 
void AnimatorController::SetCurrentState(unsigned int curState)
{
	curStateIndex = curState;
	nextStateIndex = -1;

	//blender->GetCurInterpolator()->SetAnimation(states[currentState]->GetAnimation());
	//blender->GetCurInterpolator()->SetSpeed(states[currentState]->GetSpeed());
	//blender->GetCurInterpolator()->SetIsLoop(states[currentState]->DoesItLoop());

	//blender->GetNextInterpolator()->SetAnimation(states[currentState]->GetAnimation());
	//blender->GetNextInterpolator()->SetSpeed(states[currentState]->GetSpeed());
	//blender->GetNextInterpolator()->SetIsLoop(states[currentState]->DoesItLoop());
}

void AnimatorController::SetCurrentState(State* curState)
{
	for (int i = 0; i < states.size(); ++i)
	{
		if (curState == states[i])
		{
			curStateIndex = i;
			break;
		}
	}

	nextStateIndex = -1;
	//curTransition = nullptr; //b/c this marks end of transition
	//isTransitioning = false;
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