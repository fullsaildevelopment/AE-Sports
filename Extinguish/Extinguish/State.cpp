#include "State.h"
#include "Transition.h"

void State::Update()
{
	for (int i = 0; i < transitions.size(); ++i)
	{
		transitions[i]->Update();
	}
}