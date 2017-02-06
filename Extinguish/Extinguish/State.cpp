#include "State.h"
#include "Transition.h"

//basic//
void State::Init(AnimatorController* controller, Animation* anim, bool isLooping, float animSpeed)
{
	animController = controller;
	animation = anim;
	doLoop = isLooping;
	speed = animSpeed;
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

//setters//
void State::SetAnimation(Animation* anim)
{
	animation = anim;
}