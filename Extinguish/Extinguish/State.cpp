#include "State.h"
#include "Transition.h"

//basic//
void State::Init(AnimatorController* controller, Animation* anim, bool isLooping, float animSpeed, std::string stateName)
{
	animController = controller;
	animation = anim;
	doLoop = isLooping;
	speed = animSpeed;
	name = stateName;
}

Transition* State::Update(float dt)
{
	Transition* result = nullptr;

	for (int i = 0; i < transitions.size(); ++i)
	{
		if (transitions[i]->Update(dt))
		{
			result = transitions[i];
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

//setters//
void State::SetAnimation(Animation* anim)
{
	animation = anim;
}