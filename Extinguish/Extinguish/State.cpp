#include "State.h"
#include "Transition.h"

//basic//
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

	return nullptr;
}

//getters//
Animation* State::GetAnimation()
{
	return animation;
}

//setters//
void State::SetAnimation(Animation* anim)
{
	animation = anim;
}