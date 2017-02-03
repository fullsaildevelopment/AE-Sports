#include "Transition.h"
#include "Parameter.h"

Transition::Transition()
{
	doTransition = false;
	timer = 0.0f;
}

Transition::~Transition()
{

}

void Transition::Init(State* curState, State* nextState, float exitTime, float transitionDuration)
{
	from = curState;
	to = nextState;
	this->exitTime = exitTime;
	this->transitionDuration = transitionDuration;
}

bool Transition::Update(float dt)
{
	timer += dt;

	if (!doTransition)
	{
		//if there are more than 0 conditions
		if (conditions.size())
		{
			unsigned int conditionsCount = 0;

			//check conditons
			for (int i = 0; i < conditions.size(); ++i)
			{
				if (conditions[i]->Check())
				{
					++conditionsCount;
				}
			}

			//if all are met, do transition
			if (conditionsCount == conditions.size())
			{
				doTransition = true;
				timer = 0.0f; //reset timer because new timer will be used for transition duration
			}
		}
		else if (hasExitTime)
		{
			if (timer > exitTime)
			{
				doTransition = true;
				timer = 0.0f;
			}
		}
	}
	else
	{
		//reset boolean and timer because the next frame this won't run given the state will have changed in animator controller
		doTransition = false;
		timer = 0.0f;
	}

	return doTransition;
}

void Transition::AddCondition(Parameter* parameter)
{
	conditions.push_back(parameter);
}

//getters//
float Transition::GetTransitionDuration()
{
	return transitionDuration;
}

State* Transition::GetFromState()
{
	return from;
}

State* Transition::GetToState()
{
	return to;
}

//setters//