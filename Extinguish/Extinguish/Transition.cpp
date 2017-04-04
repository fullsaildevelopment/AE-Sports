#include <iostream>
#include "Transition.h"
#include "Parameter.h"
#include "State.h"
#include "AnimatorController.h"
#include "Blender.h"

using namespace std;

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

	if (exitTime != -1)
	{
		hasExitTime = true;
	}
	else
	{
		hasExitTime = false;
	}
}

bool Transition::Update(float _dt)
{
	bool result = true;
	timer += _dt;

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
				to->GetAnimationController()->TransitionTo(this);
				//cout << to->GetName();
			}
		}
		else if (hasExitTime && to->GetAnimationController()->GetBlender()->GetCurInterpolator()->IsFinished())
		{
			if (timer > exitTime)
			{
				doTransition = true;
				timer = 0.0f;
				to->GetAnimationController()->TransitionTo(this);
				//cout << to->GetName();
			}
		}
	}
	else
	{
		//reset boolean and timer because the next frame this won't run given the state will have changed in animator controller
		//when transition is done, animator controller state needs to be set
		if (timer > transitionDuration)
		{
			to->GetAnimationController()->SetCurrentState(to);
			//cout << to->GetName() << endl;
			doTransition = false;
			timer = 0.0f;
		}

		result = false;
	}

	//if (doTransition)
	//{
	//	cout << doTransition;
	//}

	return result;
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

//to be used by client to make transition happen in the proper way with blending and everything
void Transition::SetDoTransition(bool toggle)
{
	doTransition = toggle;
}

void Transition::SetTimer(float time)
{
	timer = time;
}