#include "State.h"
#include "Transition.h"

State::~State()
{
	for (int i = 0; i < transitions.size(); ++i)
	{
		delete transitions[i];
	}
}

//basic//
void State::Init(AnimatorController* controller, Animation* anim, bool isLooping, float animSpeed, std::string stateName)
{
	animController = controller;
	animation = anim;
	doLoop = isLooping;
	speed = animSpeed;
	name = stateName;

	transitionIndex = -1;
}

Transition* State::Update(float _dt)
{
	Transition* result = nullptr;
	transitionIndex = -1;

	for (int i = 0; i < transitions.size(); ++i)
	{
		if (transitions[i]->Update(_dt))
		{
			result = transitions[i];
			transitions[i]->GetToState()->SetTransitionIndex(i);
			break;
		}
	}

	return result;
}

//misc//
void State::AddTransition(Transition* transition)
{
	transitions.push_back(transition);
}

//getters//
Animation* State::GetAnimation()
{
	return animation;
}

AnimatorController* State::GetAnimationController()
{
	return animController;
}

Transition* State::GetTransition(unsigned int index)
{
	return transitions[index];
}

bool State::DoesItLoop()
{
	return doLoop;
}

float State::GetSpeed()
{
	return speed;
}

std::string& const State::GetName()
{
	return name;
}

int State::GetTransitionIndex()
{
	return transitionIndex;
}

unsigned int State::GetTransitionsCount()
{
	return (unsigned int)transitions.size();
}


//setters//
void State::SetAnimation(Animation* anim)
{
	animation = anim;
}

void State::SetTransitionIndex(int transitionIndex)
{
	this->transitionIndex = transitionIndex;
}